/*
 * Description:  Class defining a command over the actuators for the e-puck
 */

#ifndef EPUCK_ACTUATOR_COMMAND_HPP
#define EPUCK_ACTUATOR_COMMAND_HPP

#include <core/RobotActuatorCommand.hpp>
#include "EPuckFacade.hpp"

class EPuckActuatorCommand : public RobotActuatorCommand
{
public:

  EPuckActuatorCommand();
  virtual ~EPuckActuatorCommand() {}

  int ledValue(int i) const { return mLedValues[i]; }
  int leftSpeed() const { return mSpeeds[EPuckFacade::LEFT]; }
  int rightSpeed() const { return mSpeeds[EPuckFacade::RIGHT]; }

  void setLedValue(int i, int value) { mLedValues[i] = value; }
  void setLeftSpeed(int speed) { mSpeeds[EPuckFacade::LEFT] = speed; }
  void setRightSpeed(int speed) { mSpeeds[EPuckFacade::RIGHT] = speed; }

  virtual void sendCommands() const;

  virtual void fromString(const QString &string);
  virtual void fromStringVersion3(const QString &string); // backward compatibility code
  virtual QString toString() const;

private:
  int mLedValues[EPuckFacade::NUMBER_OF_LEDS];
  int mSpeeds[2];
  EPuckFacade *mEPuckFacade;
};

#endif
