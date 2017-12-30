/*
 * Description:  Abstract class defining a condition over the sensors
 */

#ifndef ROBOT_SENSOR_CONDITION_HPP
#define ROBOT_SENSOR_CONDITION_HPP

#include <QtCore/QString>
#include <QtCore/QStringList>

class RobotSensorCondition
{
public:
  virtual ~RobotSensorCondition() {}

  virtual bool isFired() const = 0;
  virtual void fromString(const QString &string) = 0;
  virtual void fromStringVersion3(const QString &string) = 0; // backward compatibility code
  virtual QString toString() const = 0;
  virtual void reset() = 0;
  virtual void resetSensorsUseNotification() = 0;
};

#endif
