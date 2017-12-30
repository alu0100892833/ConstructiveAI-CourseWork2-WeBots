/*
 * Description:  Class defining a widget allowing to switch between three viewers
 */

#ifndef SWITCH_WIDGET_HPP
#define SWITCH_WIDGET_HPP

#include <QtWidgets/QWidget>

class Model;
class RobotConditionWidget;
class RobotStateWidget;
class RobotViewWidget;

QT_BEGIN_NAMESPACE
class QStackedLayout;
QT_END_NAMESPACE

class SwitchWidget : public QWidget
{
  Q_OBJECT

public:
  SwitchWidget(QWidget *parent = NULL);
  virtual ~SwitchWidget();
  void updateValues();

protected slots:
  void selectWidget();

protected:
  enum {StateWidget, TransitionWidget, RobotWidget};

  RobotStateWidget *mStateWidget;
  RobotConditionWidget *mConditionWidget;
  RobotViewWidget *mViewWidget;

  Model *mModel;
  QStackedLayout *mStackedLayout;
};

#endif
