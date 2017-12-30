#include "EPuckInputPacket.hpp"

#include "Camera.hpp"
#include "Device.hpp"
#include "DeviceManager.hpp"
#include "DifferentialWheels.hpp"
#include "EPuckOutputPacket.hpp"
#include "SingleValueSensor.hpp"
#include "Sensor.hpp"
#include "TripleValuesSensor.hpp"

#include <webots/remote_control.h>

#include <iostream>

#include <cstdlib>

using namespace std;

EPuckInputPacket::EPuckInputPacket(int maxSize) :
  Packet(maxSize, maxSize)
{
}

EPuckInputPacket::~EPuckInputPacket() {
}

void EPuckInputPacket::decode(int simulationTime, const EPuckOutputPacket &outputPacket) {
  // ---
  // Sensors
  // ---

  // the order of the sensors should match with EPuckOutputPacket::apply()

  int currentPos = 0;

  if (outputPacket.isAccelerometerRequested()) {
    static double calibration_k[3] = { - 9.81 / 800.0, 9.81 / 800.0, 9.81 / 800.0};
    static double calibration_offset = -2000.0;

    double values[3];
    for (int i=0; i<3; i++) {
      values[i] = readUShortAt(currentPos);
      currentPos += sizeof(unsigned short);
      // raw to SI
      values[i] = calibration_k[i] * (values[i] + calibration_offset);
    }

    TripleValuesSensor *accelerometer = DeviceManager::instance()->accelerometer();
    wbr_accelerometer_set_values(accelerometer->tag(), values);
    accelerometer->resetSensorRequested();
  }

  // read value of optional ground sensor
  if (outputPacket.areGroundSensorRequested()) {
    for (int i=0; i<3; i++) {
      SingleValueSensor *gs = DeviceManager::instance()->groundSensor(i);
      if (gs) {
        double value = readUShortAt(currentPos);
        currentPos += sizeof(unsigned short);

        wbr_distance_sensor_set_value(gs->tag(), value);
        gs->resetSensorRequested();
        gs->setLastRefreshTime(simulationTime);
      }
    }
  }

  if (outputPacket.areDistanceSensorRequested()) {
    for (int i=0; i<8; i++) {
      double value = readUShortAt(currentPos);
      currentPos += sizeof(unsigned short);

      SingleValueSensor *ds = DeviceManager::instance()->distanceSensor(i);
      wbr_distance_sensor_set_value(ds->tag(), value);
      ds->resetSensorRequested();
      ds->setLastRefreshTime(simulationTime);
    }
  }

  if (outputPacket.areLightSensorRequested()) {
    for (int i=0; i<8; i++) {
      double value = readUShortAt(currentPos);
      currentPos += sizeof(unsigned short);

      SingleValueSensor *ls = DeviceManager::instance()->lightSensor(i);
      wbr_light_sensor_set_value(ls->tag(), value);
      ls->resetSensorRequested();
      ls->setLastRefreshTime(simulationTime);
    }
  }

  if (outputPacket.isEncoderRequested()) {
    double values[2];
    for (int i=0; i<2; i++) {
      values[i] = readShortAt(currentPos);
      currentPos += sizeof(short);
    }

    wbr_differential_wheels_set_encoders(values[0], values[1]);

    DifferentialWheels *dw = DeviceManager::instance()->differentialWheels();
    dw->resetSensorRequested();
  }

  if (outputPacket.isCameraRequested()) {
    int mode = (int) readUCharAt(currentPos++);
    int wh1 = (int) readUCharAt(currentPos++); // can be width or height depending on the mode
    int wh2 = (int) readUCharAt(currentPos++); // can be width or height depending on the mode
    const unsigned char *rawImage = (const unsigned char *) &(data()[currentPos]);

    Camera *camera = DeviceManager::instance()->camera();
    camera->resetSensorRequested();

    unsigned char *rgbImage = (unsigned char *) malloc(4 * wh1 * wh2);

    if (camera->rawToBgraImage(rgbImage, rawImage, mode, wh1, wh2))
      wbr_camera_set_image(camera->tag(), (const unsigned char *) rgbImage);

    free(rgbImage);
  }
}
