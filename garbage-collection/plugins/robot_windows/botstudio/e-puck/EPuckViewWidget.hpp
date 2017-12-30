/*
 * Description:  Class defining a widget allowing to view the current values of the epuck
 */

#ifndef EPUCK_VIEW_WIDGET_HPP
#define EPUCK_VIEW_WIDGET_HPP

#include "EPuckFacade.hpp"

#include <core/RobotViewWidget.hpp>

class EPuckSlider;
class Model;

class EPuckViewWidget : public RobotViewWidget
{
public:
  EPuckViewWidget(QWidget *parent = NULL);
  virtual ~EPuckViewWidget();

  virtual void updateValues();

protected:
  EPuckSlider *mDistanceSensorSliders[EPuckFacade::NUMBER_OF_DISTANCE_SENSORS];
  EPuckSlider *mGroundSensorSliders[EPuckFacade::NUMBER_OF_GROUND_SENSORS];
  EPuckSlider *mSpeedSliders[2];
  EPuckSlider *mCameraSliders;

  Model *mModel;
};

#endif
