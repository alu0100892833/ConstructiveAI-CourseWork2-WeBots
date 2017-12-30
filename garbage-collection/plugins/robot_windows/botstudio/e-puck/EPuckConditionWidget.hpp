/*
 * Description:  Class defining a widget allowing to view the selected condition
 */

#ifndef EPUCK_CONDITION_WIDGET_HPP
#define EPUCK_CONDITION_WIDGET_HPP

#include <core/RobotConditionWidget.hpp>
#include "EPuckFacade.hpp"

class EPuckSlider;

class EPuckConditionWidget : public RobotConditionWidget
{
  Q_OBJECT

public:
  EPuckConditionWidget(QWidget *parent = NULL);
  virtual ~EPuckConditionWidget();
  virtual void setSensorCondition(RobotSensorCondition *sensorCondition);

private slots:
  void updateSensorCondition();

private:
  void blockSlidersSignals(bool block);

  EPuckSlider *mDistanceSensorSliders[EPuckFacade::NUMBER_OF_DISTANCE_SENSORS];
  EPuckSlider *mGroundSensorSliders[EPuckFacade::NUMBER_OF_GROUND_SENSORS];
  EPuckSlider *mTimerSlider;
  EPuckSlider *mCameraSliders;
};

#endif
