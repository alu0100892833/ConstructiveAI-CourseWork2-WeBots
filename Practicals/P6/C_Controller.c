/*
 * File:          C_Controller.c
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

/*
 * You may want to add macros here.
 */
#define TIME_STEP 64

#define MAX_SPEED 1000
#define LED_COUNT 10


static WbDeviceTag leds[LED_COUNT];


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
  
  /*
   * You should declare here WbDeviceTag variables for storing
   * robot devices like this:
   *  WbDeviceTag my_sensor = wb_robot_get_device("my_sensor");
   *  WbDeviceTag my_actuator = wb_robot_get_device("my_actuator");
   */
  
  /* main loop
   * Perform simulation steps of TIME_STEP milliseconds
   * and leave the loop when the simulation is over
   */
  while (wb_robot_step(TIME_STEP) != -1) {
    
    /* 
     * Read the sensors :
     * Enter here functions to read sensor data, like:
     *  double val = wb_distance_sensor_get_value(my_sensor);
     */

    /* Process sensor data here */
    double left = MAX_SPEED/2;
    double right = MAX_SPEED/2;

    /*
     * Enter here functions to send actuator commands, like:
     * wb_differential_wheels_set_speed(100.0,100.0);
     */
    wb_differential_wheels_set_speed(left, right);    
    wb_led_set(leds[1],1);
  };
  
  /* Enter your cleanup code here */

  /* This is necessary to cleanup webots resources */
  wb_robot_cleanup();
  
  return 0;
}