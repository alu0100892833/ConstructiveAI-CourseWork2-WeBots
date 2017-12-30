/*
 * Description:  Implementation of the RobotStateWidget.hpp functions
 */

#include "RobotStateWidget.hpp"

#include <QtGui/QtGui>

RobotStateWidget::RobotStateWidget(QWidget *parent) :
  QGraphicsView(parent),
  mActuatorCommand(NULL)
{
}
