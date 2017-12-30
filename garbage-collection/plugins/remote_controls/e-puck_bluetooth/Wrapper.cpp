#include "Wrapper.hpp"

#include "Camera.hpp"
#include "Communication.hpp"
#include "Device.hpp"
#include "DeviceManager.hpp"
#include "DifferentialWheels.hpp"
#include "EPuckOutputPacket.hpp"
#include "EPuckInputPacket.hpp"
#include "Led.hpp"
#include "Sensor.hpp"
#include "Serial.hpp"
#include "Time.hpp"
#include "Uploader.hpp"
#include "UploaderData.hpp"

#include <vector>
#include <iostream>

#include <cstdlib>
#include <cstdio>

using namespace std;

Communication *Wrapper::cCommunication = NULL;
Time *Wrapper::cTime = NULL;
bool Wrapper::cSuccess = true;
bool Wrapper::cCameraInitialized = false;

void Wrapper::init() {
  DeviceManager::instance();

  cCommunication = new Communication;
}

void Wrapper::cleanup() {
  delete cCommunication;

  DeviceManager::cleanup();
}

bool Wrapper::start(void *arg) {
  if (!arg)
    return false;
  DeviceManager::instance()->camera()->checkResolution();
  string port((const char *) arg);
  cCommunication->initialize(port);
  cTime = new Time();
  cSuccess = cCommunication->isInitialized();
  cCameraInitialized = false;
  return cSuccess;
}

void Wrapper::stop() {
  if (!hasFailed())
    stopActuators();

  cCommunication->cleanup();

  if (cTime) {
    delete cTime;
    cTime = NULL;
  }
}

int Wrapper::robotStep(int step) {
  // get simulation time at the beginning of this step
  int beginStepTime = cTime->elapsedTime();

  // apply to sensors
  DeviceManager::instance()->apply(beginStepTime);

  // setup the output packet
  EPuckOutputPacket outputPacket;
  outputPacket.apply(beginStepTime);

  // camera initialisation if required
  if (outputPacket.isCameraRequested())
    initializeCamera();

  // 3 trials before giving up
  for (int i=0; i<3; i++) {

    // send the output packet
    cSuccess = cCommunication->sendPacket(&outputPacket);
    if (!cSuccess) {
      cerr << "Failed to send packet to the e-puck. Retry (" << (i+1) << ")..." << endl;
      continue;
    }

    // setup and receive the input packet
    int answerSize = outputPacket.answerSize();
    EPuckInputPacket inputPacket(answerSize);
    cSuccess = cCommunication->receivePacket(&inputPacket);
    if (!cSuccess) {
      cerr << "Failed to receive packet from the e-puck. Retry (" << (i+1) << ")..." << endl;
      continue;
    }
    inputPacket.decode(beginStepTime, outputPacket);

    if (cSuccess) break;
  }
  if (!cSuccess) return 0;

  // get simulation time at the end of this step
  int endStepTime = cTime->elapsedTime();

  // according to the step duration, either wait
  // or returns the delay
  int deltaStepTime = endStepTime - beginStepTime;
  if (deltaStepTime <= step) { // the packet is sent at time
    Time::wait(step - deltaStepTime);
    return 0;
  }
  else // the delay asked is not fulfilled
    return deltaStepTime - step;
}

void Wrapper::stopActuators() {
  // reset all the requests
  DifferentialWheels *dw = DeviceManager::instance()->differentialWheels();
  dw->resetSpeedRequested();
  dw->resetEncoderRequested();

  for (int i=0; i<10; i++) {
    Led *led = DeviceManager::instance()->led(i);
    led->resetLedRequested();
  }

  vector<Device *>::const_iterator it;
  const vector<Device *> &devices = DeviceManager::instance()->devices();
  for (it=devices.begin() ; it < devices.end(); it++) {
    Sensor *s = dynamic_cast<Sensor *>(*it);
    if (s)
      s->resetSensorRequested();
  }

  // reset actuators
  differentialWheelsSetSpeed(0.0, 0.0);
  for(int i=0; i<10; i++)
    ledSet(DeviceManager::instance()->led(i)->tag(), 0);

  // send the packet
  robotStep(0);
}

void Wrapper::setSamplingPeriod(WbDeviceTag tag, int samplingPeriod) {
  Device *device = DeviceManager::instance()->findDeviceFromTag(tag);
  Sensor *sensor = dynamic_cast<Sensor *>(device);
  if (sensor) {
    sensor->setLastRefreshTime(0);
    sensor->setSamplingPeriod(samplingPeriod);
  } else
    cerr << "Wrapper::setSamplingPeriod: unknown device" << endl;
}

void Wrapper::differentialWheelsSetSpeed(double left, double right) {
  DifferentialWheels *dw = DeviceManager::instance()->differentialWheels();
  dw->setSpeedRequested();
  dw->setLeftSpeed(left);
  dw->setRightSpeed(right);
}

void Wrapper::differentialWheelsSetEncoders(double left, double right) {
  DifferentialWheels *dw = DeviceManager::instance()->differentialWheels();
  dw->setEncoderRequested();
  dw->setLeftEncoder(left);
  dw->setRightEncoder(right);
}

void Wrapper::ledSet(WbDeviceTag tag, int state) {
  Device *device = DeviceManager::instance()->findDeviceFromTag(tag);
  Led *led = dynamic_cast<Led *>(device);
  if (led) {
    led->setLedRequested();
    led->setState(state);
  }
}

void *Wrapper::findAvailablePorts(void *) {
  Serial::updatePorts();
  const vector<std::string> *comPorts = Serial::availablePorts();
  // we need to convert this to a C string to pass it to the robot window
  // which is written in C and is unable to handle a vector<std::string>
  // the C string simply list the available ports, separated by a '\n'.
  int size = comPorts->size();
  size_t count = size; // room for the '\n' characters and final '\0' character
  for(int i = 0; i < size ; i++)
    count += comPorts->at(i).length(); // room for the file names
  if (count == 0)
    // no ports found
    return NULL;

  char *port = (char *)malloc(count);
  int n = 0;
  for(int i = 0; i < size ; i++) {
    int l = comPorts->at(i).length();
    for(int j = 0; j < l; j++)
      port[n++] = comPorts->at(i)[j];
    port[n++] = '\n';
  }
  port[n-1] = '\0';
  return port;
}

void Wrapper::initializeCamera() {
  if (hasFailed() || cCameraInitialized)
    return;

  Camera *camera = DeviceManager::instance()->camera();

  char *answer = cCommunication->talk(camera->generateInitialisationCommand().c_str());

  if (answer) {
    cCameraInitialized = true;
    free(answer);
  }
  else
    cSuccess = false;
}

void *Wrapper::callCustomFunction(void *args) {
  if (args == NULL)
    return findAvailablePorts(args);

  static int uploadReturnValue = 0;
  UploaderData uploaderData = ((UploaderData *)args)[0];
  if (uploaderData.command == UPLOADER_DATA_CONNECT) {
    uploadReturnValue = Uploader::connect(uploaderData.data);
  } else if (uploaderData.command == UPLOADER_DATA_DISCONNECT) {
    Uploader::disconnect();
    uploadReturnValue = 0;
  } else if (uploaderData.command == UPLOADER_DATA_CANCEL) {
    Uploader::cancelUpload();
    uploadReturnValue = 0;
  } else  // UPLOADER_DATA_SEND_FILE
    uploadReturnValue = Uploader::send(uploaderData.robot_id, uploaderData.data, uploaderData.progress_callback);

  return &uploadReturnValue;
}
