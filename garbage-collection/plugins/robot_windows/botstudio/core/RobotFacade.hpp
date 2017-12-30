/*
 * Description:  Abstract class defining the way to get the robot information
 */

#ifndef ROBOT_FACADE_HPP
#define ROBOT_FACADE_HPP

#include <QtCore/QObject>

class RobotFacade : public QObject
{
  Q_OBJECT

public:
  virtual ~RobotFacade() {}

  virtual void update() = 0;
  virtual void stop() = 0;
  virtual void sendActuatorCommands() = 0;

signals:
  void warning(const QString &text);
};

#endif
