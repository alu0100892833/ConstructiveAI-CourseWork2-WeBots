/*
 * Description:  Implementation of the RobotObjectFactory.hpp functions
 */

#include "RobotObjectFactory.hpp"

#include <QtCore/QtGlobal>

bool RobotObjectFactory::cIsAlreadyAnInstance = false;
RobotObjectFactory *RobotObjectFactory::cInstance = NULL;

RobotObjectFactory *RobotObjectFactory::instance() {
  return cInstance;
}

void RobotObjectFactory::setInstance(RobotObjectFactory *i) {
  if (cIsAlreadyAnInstance)
    qFatal("Cannot create two instances of RobotObjectFactory");
  else {
    cInstance = i;
    cIsAlreadyAnInstance = true;
  }
}

void RobotObjectFactory::killInstance() {
  if (cInstance) {
    delete cInstance;
    cInstance = NULL;
    cIsAlreadyAnInstance = false;
  }
}

RobotObjectFactory::RobotObjectFactory(const QString &robotName) :
  mRobotName(robotName)
{
}
