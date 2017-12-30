/*
 * Description:  Implementation of the RobotConditionWidget.hpp functions
 */

#include "RobotConditionWidget.hpp"

#include <QtWidgets/QtWidgets>

RobotConditionWidget::RobotConditionWidget(QWidget *parent) :
  QGraphicsView(parent),
  mSensorCondition(NULL)
{
}
