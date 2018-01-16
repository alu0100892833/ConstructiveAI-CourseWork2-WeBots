/*
 * File:          garbage_collection.c
 * Date:          2th January 2018
 * Description:   Controller for a garbage-collection robot
 * Author:        Óscar Darias Plasencia, based on the wall-following controller.
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

#define MAX_SPEED 1000
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

#define PUSHING_THRESHOLD 150
#define OBJECT_DETECTION_THRESHOLD 80
#define TURNAROUND_COUNTER 10


/**
* Some global variables.
*/
static WbDeviceTag proximity_sensors[PS_COUNT];
static double ps_val[PS_COUNT];
static WbDeviceTag ground_sensors[GS_COUNT];
static double gs_val[GS_COUNT];
static int turning_around_left;
static int turning_around_right;


/**
* This method initialises all sensors of the robot.
*/
void epuck_init()
{
  int i;

  // initialise the counters that will act as a memory for remembering
  // that the robot was turning around (left and right)
  turning_around_left = FALSE;
  turning_around_right = FALSE;

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
    WbNodeType type = wb_device_get_node_type(dev);
    if(type == WB_NODE_DISTANCE_SENSOR && nm[0]=='g' && nm[1]=='s') {
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
  for (i=0; i<PS_COUNT; i++)
    ps_val[i] = wb_distance_sensor_get_value(proximity_sensors[i]);
  for (i=0; i<GS_COUNT; i++)
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
* Checks if the robot is stuck in a corner. First level of the subsumption architecture design.
*/
int stuck_in_a_corner() {
  if (ps_val[0] > AVOID_PROX_0 && ps_val[7] > AVOID_PROX_7)
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
* Checks if the robot is pushing garbage. Compares both front sensors with a pushing threshold (big value)
*/
int is_pushing_garbage() {
	if (ps_val[0] > PUSHING_THRESHOLD || ps_val[7] > PUSHING_THRESHOLD)
		return TRUE;
	return FALSE;
}


/*
* Once checked that it is pushing garbage, follow it by balancing both front sensors.
*/
void push_garbage_motor_values(double *left, double *right) {
	// the left one detected a bigger value, so turning right slightly
	if (ps_val[0] > ps_val[7]) {
		*right = MAX_SPEED * 0.7;
		*left = MAX_SPEED * 0.9;
	} else if (ps_val[0] < ps_val[7]) { // the left one detected a bigger value, so turning left slightly
		*left = MAX_SPEED * 0.7;
		*right = MAX_SPEED * 0.9;
	} else {	// in the unlikely situation that both sensors detect the same value, move straight
		*left = MAX_SPEED;
		*right = MAX_SPEED;
	}
}


/**
* Wanders around, being attracted by higher values of the proximity sensors.
*/
void look_for_garbage(double *left, double *right) {
	int i;
	*left = MAX_SPEED;
	*right = MAX_SPEED;
	for (i = 0; i < PS_COUNT; i++) {
		if (max_proximity_metric(i) == TRUE && ps_val[i] > OBJECT_DETECTION_THRESHOLD) {
			switch (i) {
				case 0 : *right = MAX_SPEED * 0.6; break;
    			case 1 : *right = MAX_SPEED * 0.3; break;
    			case 2 : *right = MAX_SPEED * 0.1; break;
    			case 5 : *left = MAX_SPEED * 0.1; break;
    			case 6 : *left = MAX_SPEED * 0.3; break;
    			case 7 : *left = MAX_SPEED * 0.6; break;
			}
		}
	}
}

/*
* Checks if the robot is off-limits or in garbage disposal area (black floor)
*/
int touched_wall() {
	if (gs_val[0] < BLACK_MAX || gs_val[1] < BLACK_MAX || gs_val[2] < BLACK_MAX) {
		if (gs_val[2] > gs_val[0])	// if the right sensor detects a bigger value, turn left
        	turning_around_left = TURNAROUND_COUNTER;
      	else	// if the left sensor detects a bigger value, turn right
        	turning_around_right= TURNAROUND_COUNTER;
		return TRUE;
	}
	return FALSE;
}

/*
* Decreases the turn around counter and continues turning.
* Once the counter reaches 0 (FALSE) the robot stops turning.
*/
void turn_around(double *left, double *right) {
	// keep on turning to the side it has been turning before, decreasing the counter
  	if (turning_around_left > FALSE) {
    	turning_around_left--;
    	*right = -MAX_SPEED;
    	*left = 0;
  	} else {
    	turning_around_right--;
    	*left = -MAX_SPEED;
    	*right = 0;
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

    // if it was turning around, continue
    if (turning_around_left != FALSE || turning_around_right != FALSE) {
    	turn_around(&left, &right);
    } else if (touched_wall() == TRUE) { // if arrived at the disposal area (or obstacle), turn around
    	turn_around(&left, &right);
    } else if (is_pushing_garbage() == TRUE) { // if it is pushing garbage, call the function to continue with that
    	push_garbage_motor_values(&left, &right);
    } else {	// if it is not doing anything useful, look for garbage
    	look_for_garbage(&left, &right);
    }

    
    /*
     * Enter here functions to send actuator commands
     */
    wb_differential_wheels_set_speed(left, right);    
  };
  
  /* Enter your cleanup code here */

  /* This is necessary to cleanup webots resources */
  wb_robot_cleanup();
  
  return 0;
}