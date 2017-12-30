/*
 * Description:  Class defining the accelerometer group box
 */

#ifndef ACCELEROMETER_GROUP_BOX_HPP
#define ACCELEROMETER_GROUP_BOX_HPP

#include <QtWidgets/QGroupBox>

class QVBoxLayout;
class QLabel;
class QColor;
class AccelerometerGroupBox : public QGroupBox
{
public:
  AccelerometerGroupBox(QWidget *parent=0);
  virtual ~AccelerometerGroupBox();
  void updateValues();

protected:
  QVBoxLayout *mVBox;
  QLabel *mLabels[3];
};

#endif // ACCELEROMETER_GROUP_BOX_HPP
