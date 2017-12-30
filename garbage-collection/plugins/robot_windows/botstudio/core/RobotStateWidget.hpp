/*
 * Description:  Class defining a widget allowing to view the selected state
 */

#ifndef ROBOT_STATE_WIDGET_HPP
#define ROBOT_STATE_WIDGET_HPP

#include <QtWidgets/QGraphicsView>

class Model;
class RobotActuatorCommand;

class RobotStateWidget : public QGraphicsView
{
public:
  RobotStateWidget(QWidget *parent = NULL);
  virtual ~RobotStateWidget() {}
  virtual void setActuatorCommand(RobotActuatorCommand *actuatorCommand) { mActuatorCommand = actuatorCommand; }
  RobotActuatorCommand *actuatorCommand() const { return mActuatorCommand; }

private:
  RobotActuatorCommand *mActuatorCommand;
};

#endif
