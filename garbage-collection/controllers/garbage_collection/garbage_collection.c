/*
 * File:          C_Controller_avoid.c
 * Date:          
 * Description:   
 * Author:        
 * Modifications: 
 */

/*
 * You may need to add include files like <webots/distance_sensor.h> or
 * <webots/differential_wheels.h>, etc.
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

#define MAX_SPEED 1000
#define LED_COUNT 10
#define PS_COUNT 8
#define GS_COUNT 3
#define BLACK_MAX 500

#define AVOID_PROX_0 150
#define AVOID_PROX_1 100
#define AVOID_PROX_2 200
#define AVOID_PROX_5 AVOID_PROX_0
#define AVOID_PROX_6 AVOID_PROX_1
#define AVOID_PROX_7 AVOID_PROX_2


static WbDeviceTag leds[LED_COUNT];
static WbDeviceTag proximity_sensors[PS_COUNT];
static double ps_val[PS_COUNT];
static WbDeviceTag ground_sensors[GS_COUNT];
static double gs_val[GS_COUNT];


void epuck_init()
{
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


void read_sensors()
{
  int i;
  for (i=0; i<PS_COUNT ; i++)
    ps_val[i] = wb_distance_sensor_get_value(proximity_sensors[i]);
  for (i=0; i<GS_COUNT ; i++)
    gs_val[i] = wb_distance_sensor_get_value(ground_sensors[i]);
}


void line_follow_motor_values(double *left, double *right) {
  if (gs_val[0] < BLACK_MAX) {
    *left = MAX_SPEED * 0.1;
    *right = MAX_SPEED * 0.7;
  }
  else if (gs_val[2] < BLACK_MAX) {
    *left = MAX_SPEED * 0.7;
    *right = MAX_SPEED * 0.1;
  }
  else {
    *left = MAX_SPEED * 0.5;
    *right = MAX_SPEED * 0.5;
  }
}


void avoid_motor_values(double *left, double *right) {
  // Default values
  *left = MAX_SPEED * 0.8;
  *right = MAX_SPEED * 0.8;
  if (ps_val[0] > AVOID_PROX_0 || ps_val[7] > AVOID_PROX_7) {
    // Objct detected ahead, but is it more left, or more right?
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
    avoid_motor_values(&left, &right);

    /*
     * Enter here functions to send actuator commands
     */
    wb_differential_wheels_set_speed(1000.0, 1000.0);    
    wb_led_set(leds[1],1);
  };
  
  /* Enter your cleanup code here */

  /* This is necessary to cleanup webots resources */
  wb_robot_cleanup();
  
  return 0;
}