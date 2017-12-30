/*
 * Description:  Implementation of the EPuckStateWidget.hpp functions
 */

#include "EPuckStateWidget.hpp"
#include "EPuckDrawingHelper.hpp"
#include "EPuckSlider.hpp"
#include "EPuckFacade.hpp"
#include "EPuckLedButton.hpp"
#include "EPuckActuatorCommand.hpp"

#include <core/CommonProperties.hpp>
#include <core/Model.hpp>

#include <QtWidgets/QtWidgets>

#include <cmath>

EPuckStateWidget::EPuckStateWidget(QWidget *parent) :
  RobotStateWidget(parent)
{
  EPuckDrawingHelper::initQGraphicsView(this);
  QGraphicsScene *scene = new QGraphicsScene;
  EPuckDrawingHelper::initQGraphicsScene(scene, CommonProperties::stateColor());

  int radius = EPuckDrawingHelper::internalSliderRadius();
  QSize size = EPuckDrawingHelper::internalSliderSize();

  mSpeedSliders[EPuckFacade::LEFT] = new EPuckSlider(scene, QPoint(-radius, 0), -90.0, size);
  mSpeedSliders[EPuckFacade::LEFT]->setTextLocation(EPuckSlider::Down);
  mSpeedSliders[EPuckFacade::LEFT]->setRange(-100,100);
  mSpeedSliders[EPuckFacade::LEFT]->setIndex(EPuckFacade::LEFT);
  connect(mSpeedSliders[EPuckFacade::LEFT], SIGNAL(valueChanged()), this, SLOT(updateSpeedCommand()));
  mSpeedSliders[EPuckFacade::RIGHT] = new EPuckSlider(scene, QPoint(radius, 0), -90.0, size);
  mSpeedSliders[EPuckFacade::RIGHT]->setTextLocation(EPuckSlider::Up);
  mSpeedSliders[EPuckFacade::RIGHT]->setRange(-100,100);
  mSpeedSliders[EPuckFacade::RIGHT]->setIndex(EPuckFacade::RIGHT);
  connect(mSpeedSliders[EPuckFacade::RIGHT], SIGNAL(valueChanged()), this, SLOT(updateSpeedCommand()));

  radius = EPuckDrawingHelper::ledRadius();

  for (int i=0; i<EPuckFacade::NUMBER_OF_LEDS; i++) {
    double angleRad = EPuckDrawingHelper::ledAngle(i);
    mLedButtons[i] = new EPuckLedButton;
    mLedButtons[i]->setPos(radius*cos(angleRad), -radius*sin(angleRad));
    mLedButtons[i]->setIndex(i);
    scene->addItem(mLedButtons[i]);
    connect(mLedButtons[i], SIGNAL(stateChanged()), this, SLOT(updateLedCommand()));
  }
  mLedButtons[8]->setPos(0,0);

  setScene(scene);
}

EPuckStateWidget::~EPuckStateWidget() {
  for (int i=0; i<2; i++)
    delete mSpeedSliders[i];
  for (int i=0; i<EPuckFacade::NUMBER_OF_LEDS; i++)
    delete mLedButtons[i];
}

void EPuckStateWidget::setActuatorCommand(RobotActuatorCommand *lActuatorCommand) {
  RobotStateWidget::setActuatorCommand(lActuatorCommand);

  EPuckActuatorCommand *eac = static_cast<EPuckActuatorCommand *>(actuatorCommand());

  mSpeedSliders[EPuckFacade::LEFT]->setValue(eac->leftSpeed());
  mSpeedSliders[EPuckFacade::RIGHT]->setValue(eac->rightSpeed());
  for (int i=0; i<EPuckFacade::NUMBER_OF_LEDS; i++)
    mLedButtons[i]->setValue(eac->ledValue(i));
}

void EPuckStateWidget::updateSpeedCommand() {
  EPuckSlider *slider = dynamic_cast<EPuckSlider *>(sender());
  if (slider) {
    EPuckActuatorCommand *eac = static_cast<EPuckActuatorCommand *>(actuatorCommand());
    int value = slider->value();

    if (slider->index() == EPuckFacade::LEFT)
      eac->setLeftSpeed(value);
    else
      eac->setRightSpeed(value);
  }
}

void EPuckStateWidget::updateLedCommand() {
  EPuckLedButton *button = dynamic_cast<EPuckLedButton *>(sender());
  if (button) {
    EPuckActuatorCommand *eac = static_cast<EPuckActuatorCommand *>(actuatorCommand());
    int value = button->value();
    eac->setLedValue(button->index(), value);
  }
}
