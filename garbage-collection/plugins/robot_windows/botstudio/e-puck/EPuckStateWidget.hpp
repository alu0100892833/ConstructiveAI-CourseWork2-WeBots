/*
 * Description:  Class defining a widget allowing to view the selected state
 */

#ifndef EPUCK_STATE_WIDGET_HPP
#define EPUCK_STATE_WIDGET_HPP

#include "EPuckFacade.hpp"
#include <core/RobotStateWidget.hpp>

class EPuckSlider;
class EPuckLedButton;

class EPuckStateWidget : public RobotStateWidget
{
  Q_OBJECT

public:
  EPuckStateWidget(QWidget *parent = NULL);
  virtual ~EPuckStateWidget();
  virtual void setActuatorCommand(RobotActuatorCommand *actuatorCommand);

protected slots:
  void updateSpeedCommand();
  void updateLedCommand();

protected:
  EPuckSlider *mSpeedSliders[2];
  EPuckLedButton *mLedButtons[EPuckFacade::NUMBER_OF_LEDS];
};

#endif
