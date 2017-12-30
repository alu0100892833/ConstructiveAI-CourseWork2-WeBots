/*
 * Description:  Defines an interface wrapping the libController with this library
 */

#ifndef WRAPPER_HPP
#define WRAPPER_HPP

#include <webots/types.h>

class Communication;
class Time;

class Wrapper {
public:
  // init
  static void init();
  static void cleanup();

  // mandatory functions
  static bool start(void *);
  static void stop();
  static bool hasFailed() { return !cSuccess; }
  static int robotStep(int);
  static void stopActuators();

  // redefined functions
  static void *findAvailablePorts(void *);
  static void setEncoderSamplingPeriod(int samplingPeriod) { setSamplingPeriod(0, samplingPeriod); }
  static void setSamplingPeriod(WbDeviceTag tag, int samplingPeriod);
  static void differentialWheelsSetSpeed(double left, double right);
  static void differentialWheelsSetEncoders(double left, double right);
  static void ledSet(WbDeviceTag tag, int state);

  static void *callCustomFunction(void *args);

private:
  Wrapper() {}
  virtual ~Wrapper() {}

  static void initializeCamera();

  static Communication *cCommunication;
  static Time *cTime;
  static bool cSuccess;
  static bool cCameraInitialized;
  static int *cUploadReturnValue;
};

#endif
