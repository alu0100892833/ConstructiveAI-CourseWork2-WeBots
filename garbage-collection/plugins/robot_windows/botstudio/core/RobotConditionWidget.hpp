/*
 * Description:  Class defining a widget allowing to view the selected condition
 */

#ifndef ROBOT_CONDITION_WIDGET_HPP
#define ROBOT_CONDITION_WIDGET_HPP

#include <QtWidgets/QGraphicsView>

class Model;
class RobotSensorCondition;

class RobotConditionWidget : public QGraphicsView
{
public:
  RobotConditionWidget(QWidget *parent = NULL);
  virtual ~RobotConditionWidget() {}
  virtual void setSensorCondition(RobotSensorCondition *sensorCondition) { mSensorCondition = sensorCondition;}
  virtual RobotSensorCondition *sensorCondition() { return mSensorCondition; };

private:
  RobotSensorCondition *mSensorCondition;
};

#endif
