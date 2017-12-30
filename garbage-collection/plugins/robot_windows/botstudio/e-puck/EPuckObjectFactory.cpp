/*
 * Description:  Implementation of the EPuckObjectFactory.hpp functions
 */

#include "EPuckObjectFactory.hpp"
#include "EPuckFacade.hpp"
#include "EPuckActuatorCommand.hpp"
#include "EPuckSensorCondition.hpp"
#include "EPuckConditionWidget.hpp"
#include "EPuckStateWidget.hpp"
#include "EPuckViewWidget.hpp"

EPuckObjectFactory::EPuckObjectFactory() :
  RobotObjectFactory("e-puck")
{
}

EPuckObjectFactory::~EPuckObjectFactory() {
}

RobotFacade *EPuckObjectFactory::createRobotFacade() const {
  return new EPuckFacade;
}

RobotSensorCondition *EPuckObjectFactory::createRobotSensorCondition() const {
  return new EPuckSensorCondition;
}

RobotActuatorCommand *EPuckObjectFactory::createRobotActuatorCommand() const {
  return new EPuckActuatorCommand;
}

RobotConditionWidget *EPuckObjectFactory::createRobotConditionWidget(QWidget *parent) const {
  return new EPuckConditionWidget(parent);
}

RobotStateWidget *EPuckObjectFactory::createRobotStateWidget(QWidget *parent) const {
  return new EPuckStateWidget(parent);
}

RobotViewWidget *EPuckObjectFactory::createRobotViewWidget(QWidget *parent) const {
  return new EPuckViewWidget(parent);
}
