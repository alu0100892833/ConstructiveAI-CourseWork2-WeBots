/*
 * Description:  Class allowing to get Objects related with a specific robot
 */

#ifndef EPUCK_OBJECT_FACTORY_HPP
#define EPUCK_OBJECT_FACTORY_HPP

#include <core/RobotObjectFactory.hpp>

class EPuckObjectFactory: public RobotObjectFactory
{
public:
  EPuckObjectFactory();
  virtual ~EPuckObjectFactory();

  virtual RobotFacade *createRobotFacade() const;
  virtual RobotSensorCondition *createRobotSensorCondition() const;
  virtual RobotConditionWidget *createRobotConditionWidget(QWidget *parent = NULL) const;
  virtual RobotStateWidget *createRobotStateWidget(QWidget *parent = NULL) const;
  virtual RobotViewWidget *createRobotViewWidget(QWidget *parent = NULL) const;
  virtual RobotActuatorCommand *createRobotActuatorCommand() const;
};

#endif
