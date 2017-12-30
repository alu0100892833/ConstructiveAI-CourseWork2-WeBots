/*
 * Description:  Class defining the camera group box
 */

#ifndef CAMERA_GROUP_BOX_HPP
#define CAMERA_GROUP_BOX_HPP

#include <QtWidgets/QGroupBox>

class QLabel;
class QHBoxLayout;

class CameraGroupBox : public QGroupBox
{
public:
  CameraGroupBox(QWidget *parent=0);
  virtual ~CameraGroupBox();
  void updateValues();

protected:
  QLabel *mLabel;
  QHBoxLayout *mHBox;
};

#endif // CAMERA_GROUP_BOX_HPP
