/*
 * File:          energy-damage-resources.c
 * Date:      6th January 2018
 * Description:   motivational robot that needs energy to live and has to avoid receiving damage from the obstacles of the environment
 * Author:        Óscar Darias Plasencia
 */


#include <stdio.h>
#include <math.h>
#include <webots/robot.h>

#include <webots/light_sensor.h>
#include <webots/distance_sensor.h>
#include <webots/device.h>
#include <webots/differential_wheels.h>


#define TIME_STEP 64

#define TRUE 1
#define FALSE 0

#define GS_COUNT 3
#define PS_COUNT 8
#define LS_COUNT 8
#define GS_COUNT 3

#define BLACK_MAX 500
#define LIGHT_UMBRAL 4000
#define UMBRAL_ERROR 2700
#define RECHARGE_POSITION -1

#define IDEAL_ENERGY 100
#define IDEAL_DAMAGE 0
#define FATAL_ENERGY 0
#define CRITICAL_ENERGY_NEEDS 70
#define FATAL_DAMAGE 100
#define WANDER 0
#define RECHARGE 1
#define REPAIR 2

#define AVOID_PROX_0 150
#define AVOID_PROX_1 100
#define AVOID_PROX_2 200
#define AVOID_PROX_5 AVOID_PROX_0
#define AVOID_PROX_6 AVOID_PROX_1
#define AVOID_PROX_7 AVOID_PROX_2

#define IMPACT_PROX 300
#define IMPACT_DAMAGE 5

#define MAX_SPEED 1000
#define BATTERY_DECAY 0.1
#define BATTERY_CHARGE 1
#define REPARATION 0.5

static WbDeviceTag proximity_sensors[PS_COUNT];
static WbDeviceTag ground_sensors[GS_COUNT];
static WbDeviceTag light_sensors[LS_COUNT];
static WbDeviceTag ground_sensors[GS_COUNT];
static double gs_val[GS_COUNT];
static double gs_val[GS_COUNT];
static double ps_val[PS_COUNT];
static double ls_val[LS_COUNT];

//static FILE* log_file;

// Homeostatically controlled variables
// energy decays with time and speed
static float energy;
// damage increases when the robot gets too close to an obstacle
static float damage;

// These variable will have TRUE or FALSE values, acting as a memory so the robot knows that in the previous iteration was trying to recharge/repair
static int recharging;
static int repairing;

/**
* Initialise the robot.
*/
void epuck_init() {
  int i;

  // intialise the log file with its header
  //log_file = fopen("log.csv", "w");
  //fprintf(log_file, "Iteration; Energy; Damage\n");

  // initial values for the fundamental variables
  energy = IDEAL_ENERGY;
  damage = IDEAL_DAMAGE;
  recharging = FALSE;
  repairing = FALSE;

  // initialise proximity sensors
    char prox[5] = "ps0";
    for (i = 0; i < PS_COUNT; i++) {
      prox[2] = '0' + i;
      printf("Initialising %s\n", prox);
      proximity_sensors[i] = wb_robot_get_device(prox);
      wb_distance_sensor_enable(proximity_sensors[i], TIME_STEP);
    }

    // initialise light sensors
    char light[5] = "ls0";
    for (i = 0; i < LS_COUNT; i++) {
      light[2] = '0' + i;
      printf("Initialising %s\n", light);
      light_sensors[i] = wb_robot_get_device(light);
      wb_light_sensor_enable(light_sensors[i], TIME_STEP);
    }

    // initialise ground sensors
    int n_dev = wb_robot_get_number_of_devices(); // all devices
    for (i=0; i<n_dev; i++) {
      WbDeviceTag dev = wb_robot_get_device_by_index(i);
      // Check to see if device is a ground sensor
      const char *nm = wb_device_get_name(dev);
      WbNodeType type = wb_device_get_type(dev);
      if (type==WB_NODE_DISTANCE_SENSOR && nm[0]=='g' && nm[1]=='s') {
        int idx = nm[2] - '0';
          if (idx>=0 && idx<GS_COUNT) { // we expect 3 ground sensors
            printf("Initialising %s\n", nm);
            ground_sensors[idx] = wb_robot_get_device(nm);
            wb_distance_sensor_enable(ground_sensors[idx],TIME_STEP);
          } 
      }
    }
}

/**
* Read the information of all sensors and save that information on their corresponding array.
*/
void read_sensors() {
  int i;
  for (i = 0; i < PS_COUNT; i++)
      ps_val[i] = wb_distance_sensor_get_value(proximity_sensors[i]);
    for (i = 0; i < LS_COUNT; i++)
      ls_val[i] = wb_light_sensor_get_value(light_sensors[i]);
    for (i=0; i<GS_COUNT; i++)
      gs_val[i] = wb_distance_sensor_get_value(ground_sensors[i]);
}




/**
*
* @return WANDER if no need of fulfill any resource needs, so wander around.
*   RECHARGE if the robot needs to follow the light to recharge its energy.
*   REPAIR if the robot needs to repair itself, following the line then.
*/
int determine_behaviour() {
  // if in the last iteration the robot was repairing or recharging and did not finish, continue with that
  // if, when wandering, the robot ends up in the charging area or the reparation area, take advantage of that
  if (repairing != FALSE || (gs_val[0] < BLACK_MAX && gs_val[2] < BLACK_MAX && damage > IDEAL_DAMAGE))
    return REPAIR; 
  if (recharging != FALSE || (max_light_metric() == RECHARGE_POSITION && energy < IDEAL_ENERGY))
    return RECHARGE;
  

  float energy_needs = IDEAL_ENERGY - energy;

  // if no need of repairing or recharging, just wander around
  if (damage < 60 && energy_needs < 50.0) {
    return WANDER;
  } else if (damage >= 60) {  // in case the robot needs reparation
    if (energy_needs > CRITICAL_ENERGY_NEEDS) {   
      // if the energy level is critical, the robot will need to be recharged
      // he cannot be repaired if it has no energy to look for the repairing area
      return RECHARGE;
    } else {
      // if the energy level is not critical, repair the robot
      return REPAIR;
    }
  } else {
    return RECHARGE;
  }
}

/**
* Checks if there is an object in front of the robot.
* Returns TRUE macro if there is, FALSE if there is not. 
*/
int is_object_around() {
  if (ps_val[0] > AVOID_PROX_0 || ps_val[7] > AVOID_PROX_7
        || ps_val[1] > AVOID_PROX_1 || ps_val[6] > AVOID_PROX_6
        || ps_val[2] > AVOID_PROX_2 || ps_val[5] > AVOID_PROX_5) {
    return TRUE;
  }
  return FALSE;
}

/**
* Checks if an obstacle is close enough to the robot to damage it.
* Returns TRUE macro if there is, FALSE if there is not. 
*/
int there_is_impact() {
  if (ps_val[0] > IMPACT_PROX || ps_val[7] > IMPACT_PROX
        || ps_val[1] > IMPACT_PROX || ps_val[6] > IMPACT_PROX
        || ps_val[2] > IMPACT_PROX || ps_val[5] > IMPACT_PROX
        || ps_val[3] > IMPACT_PROX || ps_val[4] > IMPACT_PROX) {
    return TRUE;
  }
  return FALSE;
}

/**
* Returns the number of the light sensor that is capturing more light.
*/
int max_light_metric() {
  int result = 0;
  int i;
  for (i = 0; i < LS_COUNT; i++) 
    if (ls_val[i] < ls_val[result])
      result = i;
  if (ls_val[result] >= LIGHT_UMBRAL - UMBRAL_ERROR)
    return RECHARGE_POSITION;
  return result;
}


/*
* Function from the original controller, given by the professor. 
* Reused for 2 of the subsumption architecture design.
* It avoids impacting an obstacle.
*/
void object_avoidance_motor_values(double *left, double *right) {
    // Default values
    *left = MAX_SPEED;
    *right = MAX_SPEED;
    if (ps_val[0] > AVOID_PROX_0 || ps_val[7] > AVOID_PROX_7) {
      // Object detected ahead, but is it more left, or more right?
        if (ps_val[0] > ps_val[7]) // More on FR sensor
          *left = -MAX_SPEED; // turn left
      else
          *right = -MAX_SPEED; // turn right
    } else if (ps_val[1] > AVOID_PROX_1)
        *left = -MAX_SPEED * 0.8; // turn left
    else if (ps_val[6] > AVOID_PROX_6)
        *right = -MAX_SPEED * 0.8; // turn right
    else if (ps_val[2] > AVOID_PROX_2)
        *left = 0; // turn left slowly
    else if (ps_val[5] > AVOID_PROX_5)
        *right = 0; // turn right slowly
}

/**
* Method to follow a line, reused to guide the robot to a repairing area.
*/
void line_follow_motor_values(double *left, double *right) {
    if (gs_val[0] < BLACK_MAX) {
      *left = MAX_SPEED * 0.01;
      *right = MAX_SPEED * 0.1;
    } else if (gs_val[2] < BLACK_MAX) {
      *left = MAX_SPEED * 0.1;
      *right = MAX_SPEED * 0.01;
    } else {
      *left = MAX_SPEED * 0.5;
      *right = MAX_SPEED * 0.5;
    }
}

/**
* This method calculates the speed that the wheels should have for the robot to go to the light.
*/
void light_follow_motor_values(double *left, double *right) {
  // Default values
    *left = MAX_SPEED;
    *right = MAX_SPEED;

  int i = max_light_metric();
  switch (i) {

    // if light is detected by the back sensors, turn around
    case 3: *right = -MAX_SPEED * 0.5;
      break;
    case 4: *left = -MAX_SPEED * 0.5;
      break;
    
    // if no sensors detect light, robot is under the light, so recharges
    case RECHARGE_POSITION: *left = 0; *right = 0; 
      energy += BATTERY_CHARGE;
      if (energy >= IDEAL_ENERGY) {
        energy = IDEAL_ENERGY;
        recharging = FALSE;
      }
      break;

    // for the rest, check if the right sensors are detecting more light than the left ones, or viceversa.
    // the robot will turn to the side where more light is detected.
    default: 
      printf("");
      double right_detection = ls_val[0] + ls_val[1] + ls_val[2];
      double left_detection = ls_val[7] + ls_val[6] + ls_val[5];
      if (left_detection < right_detection)
        *left = MAX_SPEED * 0.3;
      else
        *right = MAX_SPEED * 0.3;
  }
}

/**
* Method that specifies the recharging behaviour.
* If there is an obstacle in front of the robot, avoid it.
* If not, follow the light. The light should be detected from everywhere in the rectangle arena.
*/
void recharge_robot(double *left, double *right) {
  if (is_object_around() == TRUE) {
    object_avoidance_motor_values(left, right);
  } else {
    light_follow_motor_values(left, right);
  }
}

/**
* Method that specifies the repairing behaviour.
* If it is already in a repairing area, reduce the value of the damage variable and do not move the robot.
* If there is an obstacle, avoid it.
* For the rest, try to find a black area on the ground (repairing area).
*/
void repair_robot(double *left, double *right) {
  if (gs_val[0] < BLACK_MAX && gs_val[2] < BLACK_MAX) {
    repairing = TRUE;
    *left = 0;
    *right = 0; 
    damage -= REPARATION;
    if (damage == IDEAL_DAMAGE)  // already fully repaired
      repairing = FALSE;
  } else if (is_object_around() == TRUE) {
    object_avoidance_motor_values(left, right);
  } else {
    line_follow_motor_values(left, right);
  }
}

/*
 * This is the main program.
 * The arguments of the main function can be specified by the
 * "controllerArgs" field of the Robot node
 */
int main(int argc, char **argv)
{
  /* necessary to initialize webots stuff */
  wb_robot_init();
  epuck_init();
  int iteration = 0;

  /* main loop
   * Perform simulation steps of TIME_STEP milliseconds
   * and leave the loop when the simulation is over
   */
  while (wb_robot_step(TIME_STEP) != -1) {

    printf("\n\n\n");

    /*
     * Read the sensors :
     * Enter here functions to read sensor data, like:
     *  double val = wb_distance_sensor_get_value(my_sensor);
     */
    read_sensors();
    int behaviour = determine_behaviour();

    /* Process sensor data here */
    double left, right;
    switch (behaviour) {
      case WANDER: 
        printf("Wander around\n");
        object_avoidance_motor_values(&left, &right);
        break;
      case RECHARGE: 
        printf("Needs recharging\n");
        recharging = TRUE;
        recharge_robot(&left, &right);
        break;
      case REPAIR: 
        printf("Needs repairing\n");
        repair_robot(&left, &right);
        break;
    }

    /*
     * Enter here functions to send actuator commands, like:
     * wb_differential_wheels_set_speed(100.0,100.0);
     */
    wb_differential_wheels_set_speed(left, right);

    // robot looses energy depending on its speed
    energy -= BATTERY_DECAY * (fmax(right, left) / MAX_SPEED);

    // an impact (robot close enough) increases the received damage
    if (there_is_impact() == TRUE)
      damage += IMPACT_DAMAGE;

    // log status
    //fprintf(log_file, "%d; %f; %f\n", iteration, energy, damage);

    // check if the robot ran out of energy
    if (energy <= FATAL_ENERGY) {
      printf("Robot without battery\n");
      wb_differential_wheels_set_speed(0.0, 0.0);
      break;
    } 

    // check if the robot has received too much damage and its destroyed
    if (damage >= FATAL_DAMAGE) {
      printf("Robot destroyed\n");
      wb_differential_wheels_set_speed(0.0, 0.0);
      break;
    }

    iteration++;
  };

  /* Enter your cleanup code here */
  //fclose(log_file);

  /* This is necessary to cleanup webots resources */
  wb_robot_cleanup();

  return 0;
}
