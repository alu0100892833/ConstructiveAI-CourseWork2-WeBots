/*
 * Description:  Class defining a widget allowing to view the current values of the robot
 */

#ifndef ROBOT_VIEW_WIDGET_HPP
#define ROBOT_VIEW_WIDGET_HPP

#include <QtWidgets/QGraphicsView>

class Model;

class RobotViewWidget : public QGraphicsView
{
public:
  RobotViewWidget(QWidget *parent = NULL);
  virtual ~RobotViewWidget() {}

  virtual void updateValues() = 0;
};

#endif
