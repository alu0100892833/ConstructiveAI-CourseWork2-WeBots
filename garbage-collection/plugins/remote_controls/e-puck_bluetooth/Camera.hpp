/*
 * Description:  Abstraction of a camera
 */

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Sensor.hpp"

#include <string>

class Camera : public Sensor {
public:

  // Device Manager is responsible to create/destroy devices
  Camera(WbDeviceTag tag);
  virtual ~Camera() {}

  int width() const { return mWidth; }
  int height() const { return mHeight; }
  void checkResolution() const;

  std::string generateInitialisationCommand() const;
  bool rawToBgraImage(unsigned char *bgraImage, const unsigned char *rawImage, int mode, int wh1, int wh2) const;

private:
  int mWidth;
  int mHeight;

  bool mLinearCamera;
  int mZoom;

  void computeZoom();
};

#endif
