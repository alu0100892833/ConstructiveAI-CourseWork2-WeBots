/*
 * Description:  Facade of an e-puck. A call of the update function updates the representation using the Webots API
 */

#ifndef EPUCK_REPRESENTATION_HPP
#define EPUCK_REPRESENTATION_HPP

#include <webots/types.h>

#define NUMBER_OF_LEDS 10
#define NUMBER_OF_DISTANCE_SENSORS 8
#define NUMBER_OF_LIGHT_SENSORS 8
#define NUMBER_OF_GROUND_SENSORS 3

enum {LEFT, RIGHT};

typedef struct _Device {
  WbDeviceTag tag;
  int samplingPeriod;
  union {
    int iValue;
    double dValue;
    const double *dValues;
    const unsigned char *image;
  };
} Device;

class EPuckRepresentation {

public:
  static EPuckRepresentation *instance();
  static void killInstance();
  void update();

  int ledValue(int id) const;
  double distanceSensorValue(int id) const;
  double lightSensorValue(int id) const;
  double groundSensorValue(int id) const;
  const double *accelerometerValues() const;
  const unsigned char *cameraValues() const;
  double rightSpeed() const;
  double leftSpeed() const;
  double rightEncoderValue() const;
  double leftEncoderValue() const;
  int cameraWidth() const;
  int cameraHeight() const;

  bool isDistanceSensorEnabled(int id) const;
  bool isLightSensorEnabled(int id) const;
  bool isGroundSensorEnabled(int id) const;
  bool isAccelerometerEnabled() const;
  bool isCameraEnabled() const;
  bool areEncodersEnabled() const;

  bool groundSensorsExist() { return mGroundSensorsExist; }

  void enableAllSensors();

protected:
  static EPuckRepresentation *cInstance;

  EPuckRepresentation(int);
  virtual ~EPuckRepresentation();

  Device mLeds[NUMBER_OF_LEDS];
  Device mDistanceSensors[NUMBER_OF_DISTANCE_SENSORS];
  Device mLightSensors[NUMBER_OF_LIGHT_SENSORS];
  Device mGroundSensors[NUMBER_OF_GROUND_SENSORS];
  Device mAccelerometer;
  Device mCamera;

  bool mGroundSensorsExist;

  double mEncoderValues[2];
  int mEncodersSamplingValues;
  double mSpeedValues[2];
  int mCameraWidth;
  int mCameraHeight;
};

#endif
