/*
 * File:          C_Controller_avoid.c
 * Date:          29th December 2017
 * Description:   Controller for a wall-following robot
 * Author:        Óscar Darias Plasencia, based on the C_Controller_Avoid.c given by the professor.
 * Modifications: Just recicled the avoiding object part, and added more subsumption levels.
 */


#include <stdio.h>
 
#include <webots/robot.h>
#include <webots/differential_wheels.h>
#include <webots/led.h>
#include <webots/device.h>
#include <webots/distance_sensor.h>

/*
 * You may want to add macros here.
 */
#define TIME_STEP 64

#define TRUE 1
#define FALSE 0

#define WALL_LEFT 6
#define WALL_RIGHT 7
#define NOT_FOLLOWING_WALL 9
#define ITERATIONS_TO_LOOSE_WALL 40
#define TURNING_ITERATIONS 5

#define MAX_SPEED 1000
#define LED_COUNT 10
#define PS_COUNT 8
#define GS_COUNT 3
#define BLACK_MAX 500

#define AVOID_PROX_0 150
#define AVOID_PROX_1 100
#define AVOID_PROX_2 200
#define AVOID_PROX_3 220
#define AVOID_PROX_4 AVOID_PROX_3
#define AVOID_PROX_5 AVOID_PROX_2
#define AVOID_PROX_6 AVOID_PROX_1
#define AVOID_PROX_7 AVOID_PROX_0


/**
* Some global variables.
*/
static WbDeviceTag leds[LED_COUNT];
static WbDeviceTag proximity_sensors[PS_COUNT];
static double ps_val[PS_COUNT];
static WbDeviceTag ground_sensors[GS_COUNT];
static double gs_val[GS_COUNT];
static int following;     // this variable specifies if the robot has been following a wall (WALL_LEFT if it had the wall on the left, WALL_RIGHT the opposite and NOT_FOLLOWING_WALL if neither)
static int wall_following_counter;    // robot might get lost for a couple of iterations, but it might still be following a wall. This variable is a counter for the number of iterations before considering that the robot is no longer following a wall
static int turning_counter;
static char turning_direction;


/**
* This method initialises all sensors of the robot.
*/
void epuck_init()
{
  following = NOT_FOLLOWING_WALL;
  wall_following_counter = ITERATIONS_TO_LOOSE_WALL;
  int i;

  // initialise LEDs
  char ledname[5] = "led0";
  for (i=0; i<LED_COUNT; i++) {
    ledname[3] = '0' + i;
    printf("Initialising %s\n", ledname);
    leds[i] = wb_robot_get_device(ledname);
  }

  // initialise proxmity sensors
  char proxname[5] = "ps0";
  for (i=0; i<PS_COUNT; i++) {
    proxname[2] = '0' + i;
    printf("Initialising %s\n", proxname);
    proximity_sensors[i] = wb_robot_get_device(proxname);
    wb_distance_sensor_enable(proximity_sensors[i],TIME_STEP);
  }

  // initialise ground sensors
  // Since the ground sensors may not be present, we probe
  // the available sensors for any ground sensors.
  // You may wish to assume they are present and loop through them,
  // like the LED initialisation above.
  int n_dev = wb_robot_get_number_of_devices(); // all devices
  for (i=0; i<n_dev; i++) {
    WbDeviceTag dev = wb_robot_get_device_by_index(i);
    // Check to see if device is a ground sensor
    const char *nm = wb_device_get_name(dev);
	// NB Webots8: In the next line you will need change "wb_device_get_type" to "wb_device_get_node_type"
    WbNodeType type = wb_device_get_type(dev);
    if(type==WB_NODE_DISTANCE_SENSOR && nm[0]=='g' && nm[1]=='s') {
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
  for (i=0; i<PS_COUNT ; i++)
    ps_val[i] = wb_distance_sensor_get_value(proximity_sensors[i]);
  for (i=0; i<GS_COUNT ; i++)
    gs_val[i] = wb_distance_sensor_get_value(ground_sensors[i]);
}

/**
* Checks if there is an object in front of the robot.
* Returns TRUE macro if there is, FALSE if there is not. 
*/
int is_there_an_obstacle() {
  	if (ps_val[0] > AVOID_PROX_0 || ps_val[7] > AVOID_PROX_7
        /*|| ps_val[1] > AVOID_PROX_1 || ps_val[6] > AVOID_PROX_6
        || ps_val[2] > AVOID_PROX_2 || ps_val[5] > AVOID_PROX_5*/) {
    	return TRUE;
  	}
  	return FALSE;
}

/*
* Checks whether or not the robot is correctly following a wall in a given moment.
* For that, checks the values of the side proximity sensors (2 and 5).
* If the sensors do not return the expected results, then the wall following counter is decreased.
*/
int is_following_a_wall() {
  	if (max_proximity_metric(2) == TRUE && ps_val[2] > AVOID_PROX_2) {
    	following = WALL_RIGHT;
    	wall_following_counter = ITERATIONS_TO_LOOSE_WALL;
    	return TRUE;
  	} else if (max_proximity_metric(5) == TRUE && ps_val[5] > AVOID_PROX_5) {
    	following = WALL_LEFT;
    	wall_following_counter = ITERATIONS_TO_LOOSE_WALL;
    	return TRUE;
  	} else {
    	wall_following_counter -= 1;
    	return FALSE;
  	}
}

/*
* Checks if the robot is stuck in a corner. First level of the subsumption architecture design.
*/
int stuck_in_a_corner() {
  if (ps_val[0] > AVOID_PROX_0 && ps_val[7] > AVOID_PROX_7 /*&& ps_val[1] > AVOID_PROX_1 && ps_val[6] > AVOID_PROX_6*/)
    return TRUE;
  return FALSE;
}

/*
* Function from the original controller, given by the professor. 
* Reused for 2 of the subsumption architecture design.
* It avoids impacting an obstacle.
*/
void avoid_motor_values(double *left, double *right) {
  // Default values
  *left = MAX_SPEED * 0.8;
  *right = MAX_SPEED * 0.8;
  if (ps_val[0] > AVOID_PROX_0 || ps_val[7] > AVOID_PROX_7) {
    // Object detected ahead, but is it more left, or more right?
    if (ps_val[0] > ps_val[7]) // More on FR sensor
      *left = -MAX_SPEED * 0.8; // turn left
    else
      *right = -MAX_SPEED * 0.8; // turn right
  }
  else if (ps_val[1] > AVOID_PROX_1)
      *left = -MAX_SPEED * 0.8; // turn left
  else if (ps_val[6] > AVOID_PROX_6)
      *right = -MAX_SPEED * 0.8; // turn right
  else if (ps_val[2] > AVOID_PROX_2)
      *left = 0; // turn left slowly
  else if (ps_val[5] > AVOID_PROX_5)
      *right = 0; // turn right slowly
}

/*
* Check if the specified sensor is detecting the highest proximity value.
*/
int max_proximity_metric(int sensor) {
  int i;
  for (i = 0; i < PS_COUNT; i++) {
    if (i != sensor && ps_val[sensor] < ps_val[i])
      return FALSE;
  }
  return TRUE;
}

/**
* Reads the information on the proximity sensors to find the one that is detecting the highest proximity value.
* Goes that direction, hoping to find a wall. Level 3 of the subsumption architecture design.
*/
void look_for_a_wall(double *left, double *right) {
  int i;
  for (i = 0; i < PS_COUNT; i++)
    if (max_proximity_metric(i) == TRUE)
      break;
  switch (i) {
    case 0 : *left = MAX_SPEED; *right = MAX_SPEED * 0.8; break;
    case 1 : *left = MAX_SPEED; *right = MAX_SPEED * 0.5; break;
    case 2 : *left = MAX_SPEED; *right = MAX_SPEED * 0.1; break;
    case 5 : *left = MAX_SPEED * 0.1; *right = MAX_SPEED; break;
    case 6 : *left = MAX_SPEED * 0.5; *right = MAX_SPEED; break;
    case 7 : *left = MAX_SPEED * 0.8; *right = MAX_SPEED; break;
    default: *left = MAX_SPEED; *right = MAX_SPEED; break;
  }
}

/*
* If stuck in a corner, this function allows to escape.
* Checks which side of the robot has the biggest proximity value and assumes that the wall will be on that side.
*/
void escape_from_corner(double *left, double *right) {
	turning_counter--;
	int left_value = ps_val[7] + ps_val[6] + ps_val[5];
	int right_value = ps_val[0] + ps_val[1] + ps_val[2];
	if (left_value > right_value) {
		*right = -MAX_SPEED;
    	*left = MAX_SPEED;
	} else {
		*right = MAX_SPEED;
    	*left = -MAX_SPEED;
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
  
  /* main loop
   * Perform simulation steps of TIME_STEP milliseconds
   * and leave the loop when the simulation is over
   */
  while (wb_robot_step(TIME_STEP) != -1) {
    
    /* 
     * Read the sensors :
     * Enter here functions to read sensor data
     */
    read_sensors();

    /* Process sensor data here */
    double left, right;

    if (turning_counter != FALSE) {				  /* If it was turning, continue with that */
    	escape_from_corner(&left, &right);
    } else if (stuck_in_a_corner() == TRUE) {     /* Level 1 ----> avoid getting stuck in a corner */
    	turning_counter = TURNING_ITERATIONS;
      	escape_from_corner(&left, &right);
    } else if (is_there_an_obstacle() == TRUE) {  /* Level 2 ----> avoid obstacles */
      	avoid_motor_values(&left, &right);
    } else if (is_following_a_wall() == FALSE) {  /* Level 3 ----> make sure that it is following a wall */
      	look_for_a_wall(&left, &right);
    } else {                                      /* Level 4 ----> just continue in a straight line */
      	left = MAX_SPEED;
      	right = MAX_SPEED;
    }

    /* In case the robot has been "lost" for too many iterations, then we can declare that it is no longer following any wall */
    if (wall_following_counter == 0)
      following = NOT_FOLLOWING_WALL;
    
    /*
     * Enter here functions to send actuator commands
     */
    wb_differential_wheels_set_speed(left, right);    
    wb_led_set(leds[1],1);
  };
  
  /* Enter your cleanup code here */

  /* This is necessary to cleanup webots resources */
  wb_robot_cleanup();
  
  return 0;
}