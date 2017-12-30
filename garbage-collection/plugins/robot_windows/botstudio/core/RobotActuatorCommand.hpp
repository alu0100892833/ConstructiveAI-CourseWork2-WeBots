/*
 * Description:  Abstract class defining a command over the actuators
 */

#ifndef ROBOT_ACTUATOR_COMMAND_HPP
#define ROBOT_ACTUATOR_COMMAND_HPP

#include <QtCore/QString>
#include <QtCore/QStringList>

class RobotActuatorCommand
{
public:
  virtual ~RobotActuatorCommand() {}

  virtual void sendCommands() const = 0;
  virtual void fromString(const QString &string) = 0;
  virtual void fromStringVersion3(const QString &string) = 0; // backward compatibility code
  virtual QString toString() const = 0;
};

#endif
