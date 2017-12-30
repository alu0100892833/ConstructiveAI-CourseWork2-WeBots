#include "EPuckRepresentation.hpp"

#include <webots/accelerometer.h>
#include <webots/camera.h>
#include <webots/device.h>
#include <webots/differential_wheels.h>
#include <webots/distance_sensor.h>
#include <webots/led.h>
#include <webots/light_sensor.h>
#include <webots/robot.h>

#include <cmath>
#include <cstdio>

// singleton design pattern stuff
EPuckRepresentation *EPuckRepresentation::cInstance = 0;

// constructor
// init the devices according to their names
EPuckRepresentation::EPuckRepresentation(int dummy) {
  mGroundSensorsExist = false;

  int i;

  // init leds
  char ledName[] = "led0";
  for (i = 0; i<NUMBER_OF_LEDS; i++) {
    ledName[3] = '0' + i;
    mLeds[i].tag = wb_robot_get_device(ledName);
    mLeds[i].iValue = 0;
  }

  // init distance sensors
  char psName[] = "ps0";
  for (i = 0; i<NUMBER_OF_DISTANCE_SENSORS; i++) {
    psName[2] = '0' + i;
    mDistanceSensors[i].tag = wb_robot_get_device(psName);
    mDistanceSensors[i].dValue = -1.0;
  }

  // init light sensors
  char lsName[] = "ls0";
  for (i = 0; i<NUMBER_OF_LIGHT_SENSORS; i++) {
    lsName[2] = '0' + i;
    mLightSensors[i].tag = wb_robot_get_device(lsName);
    mLightSensors[i].dValue = -1.0;
  }

  // init the accelerometer
  mAccelerometer.tag = wb_robot_get_device("accelerometer");
  mAccelerometer.dValues = 0;

  // init the camera
  mCameraWidth = 0;
  mCameraHeight = 0;
  mCamera.tag = wb_robot_get_device("camera");
  mCamera.image = 0;
  if (mCamera.tag) {
    mCameraWidth = wb_camera_get_width(mCamera.tag);
    mCameraHeight = wb_camera_get_height(mCamera.tag);
  }

  // init the differential wheel stuff
  mEncoderValues[LEFT] = 0.0;
  mEncoderValues[RIGHT] = 0.0;
  mSpeedValues[LEFT] = 0.0;
  mSpeedValues[RIGHT] = 0.0;


  // init optional devices
  for (i = 0; i<NUMBER_OF_GROUND_SENSORS; i++) {
    mGroundSensors[i].tag = 0;
  }

  WbDeviceTag tag;
  WbNodeType deviceType;
  int groundSensorIndex, matchedItems = 0;
  int numberOfDevices = wb_robot_get_number_of_devices();
  for (int index = 0; index < numberOfDevices; index++) {
    tag = wb_robot_get_device_by_index(index);
    deviceType = wb_device_get_node_type(tag);
    if (deviceType == WB_NODE_DISTANCE_SENSOR) {
      const char *deviceName = wb_device_get_name(tag);
      matchedItems = sscanf(deviceName, "gs%d", &groundSensorIndex);
      if (matchedItems > 0) {
        // init ground sensors
        if (groundSensorIndex < NUMBER_OF_GROUND_SENSORS && mGroundSensors[groundSensorIndex].tag == 0) {
          mGroundSensors[groundSensorIndex].tag = tag;
          mGroundSensors[groundSensorIndex].dValue = -1.0;
          mGroundSensorsExist = true;
        }
      }
    }
  }
}

// destructor
EPuckRepresentation::~EPuckRepresentation() {
  // nothing to delete
}

// singleton design pattern stuff
EPuckRepresentation *EPuckRepresentation::instance() {
  if (!cInstance)
    cInstance = new EPuckRepresentation(0);
  return cInstance;
}

// singleton design pattern stuff
void EPuckRepresentation::killInstance() {
  delete cInstance;
  cInstance = 0;
}

// update the values of the different components
void EPuckRepresentation::update() {

  int i;

  // update the leds state
  for (i = 0; i<NUMBER_OF_LEDS; i++)
    if (mLeds[i].tag)
      mLeds[i].iValue = wb_led_get(mLeds[i].tag);

  // update the distance sensors state
  for (i = 0; i<NUMBER_OF_DISTANCE_SENSORS; i++) {
    if (mDistanceSensors[i].tag) {
      mDistanceSensors[i].samplingPeriod = wb_distance_sensor_get_sampling_period(mDistanceSensors[i].tag);
      if (mDistanceSensors[i].samplingPeriod > 0)
        mDistanceSensors[i].dValue = wb_distance_sensor_get_value(mDistanceSensors[i].tag);
      else
        mDistanceSensors[i].dValue = -1.0;
    }
  }

  // update the light sensors state
  for (i = 0; i<NUMBER_OF_LIGHT_SENSORS; i++) {
    if (mLightSensors[i].tag) {
      mLightSensors[i].samplingPeriod = wb_light_sensor_get_sampling_period(mLightSensors[i].tag);
      if (mLightSensors[i].samplingPeriod > 0)
        mLightSensors[i].dValue = wb_light_sensor_get_value(mLightSensors[i].tag);
      else
        mLightSensors[i].dValue = -1.0;
    }
  }

  // update the ground sensors state
  for (i = 0; i<NUMBER_OF_GROUND_SENSORS; i++) {
    if (mGroundSensors[i].tag) {
      mGroundSensors[i].samplingPeriod = wb_distance_sensor_get_sampling_period(mGroundSensors[i].tag);
      if (mGroundSensors[i].samplingPeriod > 0)
        mGroundSensors[i].dValue = wb_distance_sensor_get_value(mGroundSensors[i].tag);
      else
        mGroundSensors[i].dValue = -1.0;
    }
  }

  // update the accelerometer state
  if (mAccelerometer.tag) {
    mAccelerometer.samplingPeriod = wb_accelerometer_get_sampling_period(mAccelerometer.tag);
    if (mAccelerometer.samplingPeriod > 0)
      mAccelerometer.dValues = wb_accelerometer_get_values(mAccelerometer.tag);
    else
      mAccelerometer.dValues = 0;
  }

  // update the camera state
  if (mCamera.tag) {
    mCamera.samplingPeriod = wb_camera_get_sampling_period(mCamera.tag);
    if (mCamera.samplingPeriod > 0)
      mCamera.image = wb_camera_get_image(mCamera.tag);
    else
      mCamera.image = 0;
  }

  // update the differential wheels stuff
  mEncodersSamplingValues = wb_differential_wheels_get_encoders_sampling_period();
  if (mEncodersSamplingValues > 0) {
    mEncoderValues[LEFT] = wb_differential_wheels_get_left_encoder();
    mEncoderValues[RIGHT] = wb_differential_wheels_get_right_encoder();
  } else {
    mEncoderValues[LEFT] = 0.0;
    mEncoderValues[RIGHT] = 0.0;
  }
  mSpeedValues[LEFT] = wb_differential_wheels_get_left_speed();
  mSpeedValues[RIGHT] = wb_differential_wheels_get_right_speed();
}

// getters
int EPuckRepresentation::ledValue(int id) const {
  if (id >= 0 && id < NUMBER_OF_LEDS)
    return mLeds[id].iValue;
  return 0;
}

double EPuckRepresentation::distanceSensorValue(int id) const {
  if (id >= 0 && id < NUMBER_OF_DISTANCE_SENSORS)
    return mDistanceSensors[id].dValue;
  return -1.0;
}

double EPuckRepresentation::lightSensorValue(int id) const {
  if (id >= 0 && id < NUMBER_OF_LIGHT_SENSORS)
    return mLightSensors[id].dValue;
  return -1.0;
}

double EPuckRepresentation::groundSensorValue(int id) const {
  if (id >= 0 && id < NUMBER_OF_GROUND_SENSORS && mGroundSensors[id].tag)
    return mGroundSensors[id].dValue;
  return -1.0;
}

const double *EPuckRepresentation::accelerometerValues() const {
  return mAccelerometer.dValues;
}

const unsigned char *EPuckRepresentation::cameraValues() const {
  return mCamera.image;
}

double EPuckRepresentation::rightSpeed() const {
  return mSpeedValues[RIGHT];
}

double EPuckRepresentation::leftSpeed() const {
  return mSpeedValues[LEFT];
}

double EPuckRepresentation::rightEncoderValue() const {
  return mEncoderValues[RIGHT];
}

double EPuckRepresentation::leftEncoderValue() const {
  return mEncoderValues[LEFT];
}

int EPuckRepresentation::cameraWidth() const {
  return mCameraWidth;
}

int EPuckRepresentation::cameraHeight() const {
  return mCameraHeight;
}

bool EPuckRepresentation::isDistanceSensorEnabled(int id) const {
  bool ret = false;
  if (id >= 0 && id < NUMBER_OF_DISTANCE_SENSORS)
    ret = (mDistanceSensors[id].samplingPeriod > 0);
  return ret;
}

bool EPuckRepresentation::isLightSensorEnabled(int id) const {
  bool ret = false;
  if (id >= 0 && id < NUMBER_OF_LIGHT_SENSORS)
    ret = (mLightSensors[id].samplingPeriod > 0);
  return ret;
}

bool EPuckRepresentation::isGroundSensorEnabled(int id) const {
  bool ret = false;
  if (id >= 0 && id < NUMBER_OF_GROUND_SENSORS && mGroundSensors[id].tag)
    ret = (mGroundSensors[id].samplingPeriod > 0);
  return ret;
}

bool EPuckRepresentation::isAccelerometerEnabled() const {
  return (mAccelerometer.samplingPeriod > 0);
}

bool EPuckRepresentation::isCameraEnabled() const {
  return (mCamera.samplingPeriod > 0);
}

bool EPuckRepresentation::areEncodersEnabled() const {
  return (mEncodersSamplingValues > 0);
}

void EPuckRepresentation::enableAllSensors() {
  int basicTimeStep = floor(wb_robot_get_basic_time_step());
  int i;

  for (i = 0; i<NUMBER_OF_DISTANCE_SENSORS; i++)
    wb_distance_sensor_enable(mDistanceSensors[i].tag, basicTimeStep);

  for (i = 0; i<NUMBER_OF_LIGHT_SENSORS; i++)
    wb_light_sensor_enable(mLightSensors[i].tag, basicTimeStep);

  for (i = 0; i<NUMBER_OF_GROUND_SENSORS && mGroundSensors[i].tag; i++)
    wb_distance_sensor_enable(mGroundSensors[i].tag, basicTimeStep);

  wb_accelerometer_enable(mAccelerometer.tag, basicTimeStep);
  wb_camera_enable(mCamera.tag, basicTimeStep);
  wb_differential_wheels_enable_encoders(basicTimeStep);
}
