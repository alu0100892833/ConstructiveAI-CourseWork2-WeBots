#include "EPuckOutputPacket.hpp"

#include "Camera.hpp"
#include "Device.hpp"
#include "DeviceManager.hpp"
#include "DifferentialWheels.hpp"
#include "Led.hpp"
#include "Sensor.hpp"
#include "SingleValueSensor.hpp"
#include "TripleValuesSensor.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

//#include <webots/camera.h>

using namespace std;

EPuckOutputPacket::EPuckOutputPacket() :
  Packet(52*39*2 + 3 + 80),
  mAnswerSize(0),
  mDistanceSensorRequested(false),
  mGroundSensorRequested(false),
  mLightSensorRequested(false),
  mAccelerometerRequested(false),
  mCameraRequested(false),
  mEncoderRequested(false)
{
}

EPuckOutputPacket::~EPuckOutputPacket() {
}

void EPuckOutputPacket::clear() {
  Packet::clear();

  mAnswerSize = 0;

  mDistanceSensorRequested = false;
  mGroundSensorRequested = false;
  mLightSensorRequested = false;
  mAccelerometerRequested = false;
  mCameraRequested = false;
  mEncoderRequested = false;
}

void EPuckOutputPacket::apply(int simulationTime) {

  // ---
  // Actuators
  // ---

  // send the differential wheels commands if required
  DifferentialWheels *dw = DeviceManager::instance()->differentialWheels();
  if (dw->isSpeedRequested()) {
    append(static_cast<char>(-'D'));
    append(static_cast<short>(dw->leftSpeed()));
    append(static_cast<short>(dw->rightSpeed()));
    dw->resetSpeedRequested();
  }
  if (dw->isEncoderRequested()) {
    append(static_cast<char>(-'P'));
    append(static_cast<short>(dw->leftEncoder()));
    append(static_cast<short>(dw->rightEncoder()));
    dw->resetEncoderRequested();
  }

  // send the led commands if required
  for (int i=0; i<10; i++) {
    Led *led = DeviceManager::instance()->led(i);
    if (led->isLedRequested()) {
      append(static_cast<char>(-'L'));
      append(static_cast<char>(led->index()));
      append(static_cast<char>(led->state()));
      led->resetLedRequested();
    }
  }

  // ---
  // Sensors
  // ---

  // the order of the sensors should match with EPuckInputPacket::decode()

  // accelerometer management
  TripleValuesSensor *accelerometer = DeviceManager::instance()->accelerometer();
  if (accelerometer->isSensorRequested()) {
    mAccelerometerRequested = true;
    append(static_cast<char>(-'a'));
    mAnswerSize += 3 * sizeof(short);
  }

  // manage the optional ground sensors
  // (send the command if at least one ground sensor is required)
  for (int i=0; i<3; i++) {
    SingleValueSensor *gs = DeviceManager::instance()->groundSensor(i);
    if (gs && gs->isSensorRequested()) {
      mGroundSensorRequested = true;
      append(static_cast<char>(-'M'));
      mAnswerSize += 3 * sizeof(short);
      break;
    }
  }

  // manage the distance sensors
  // (send the command if at least one distance sensor is required)
  for (int i=0; i<8; i++) {
    SingleValueSensor *ds = DeviceManager::instance()->distanceSensor(i);
    if (ds->isSensorRequested()) {
      mDistanceSensorRequested = true;
      append(static_cast<char>(-'N'));
      mAnswerSize += 8 * sizeof(short);
      break;
    }
  }

  // manage the light sensors
  // (send the command if at least one light sensor is required)
  for (int i=0; i<8; i++) {
    SingleValueSensor *ls = DeviceManager::instance()->lightSensor(i);
    if (ls->isSensorRequested()) {
      mLightSensorRequested = true;
      append(static_cast<char>(-'O'));
      mAnswerSize += 8 * sizeof(short);
      break;
    }
  }

  // differential wheels encoder management
  if (dw->isSensorRequested()) {
    mEncoderRequested = true;
    append(static_cast<char>(-'Q'));
    mAnswerSize += 2 * sizeof(short);
  }

  // camera management
  // it's better to put the camera at the end in case of
  // retrieval after transmission troubles
  Camera *camera = DeviceManager::instance()->camera();
  if (camera->isSensorRequested()) {
    mCameraRequested = true;
    append(static_cast<char>(-'I'));
    mAnswerSize += 3 + 2 * camera->width() * camera->height();
  }

  // This is require to end the packet
  // even if the size is correct
  append('\0');
}
