/*
 * Description:  Implementation of the SwitchWidget.hpp functions
 */

#include "SwitchWidget.hpp"
#include "Model.hpp"
#include "Automaton.hpp"
#include "State.hpp"
#include "Transition.hpp"
#include "RobotActuatorCommand.hpp"
#include "RobotSensorCondition.hpp"
#include "RobotConditionWidget.hpp"
#include "RobotStateWidget.hpp"
#include "RobotViewWidget.hpp"
#include "RobotObjectFactory.hpp"

#include <QtWidgets/QStackedLayout>

SwitchWidget::SwitchWidget(QWidget *parent) :
  QWidget(parent)
{
  mModel = Model::instance();

  mStateWidget = RobotObjectFactory::instance()->createRobotStateWidget(this);
  mConditionWidget = RobotObjectFactory::instance()->createRobotConditionWidget(this);
  mViewWidget = RobotObjectFactory::instance()->createRobotViewWidget(this);

  mStackedLayout = new QStackedLayout;
  mStackedLayout->addWidget(mStateWidget);
  mStackedLayout->addWidget(mConditionWidget);
  mStackedLayout->addWidget(mViewWidget);

  setLayout(mStackedLayout);

  selectWidget();
  connect(mModel->automaton(), SIGNAL(selectionChanged()), this, SLOT(selectWidget()));
}

SwitchWidget::~SwitchWidget() {
  delete mStateWidget;
  delete mConditionWidget;
  delete mViewWidget;
  delete mStackedLayout;
}

void SwitchWidget::selectWidget() {
  AutomatonObject *object = mModel->automaton()->findSelectedObject();
  State *state = dynamic_cast<State *> (object);
  Transition *transition = dynamic_cast<Transition *> (object);

  if (state) {
    RobotActuatorCommand *actuatorCommand = state->actuatorCommand();
    mStateWidget->setActuatorCommand(actuatorCommand);
    mStackedLayout->setCurrentIndex(StateWidget);
  }
  else if (transition) {
    RobotSensorCondition *sensorCondition = transition->sensorCondition();
    mConditionWidget->setSensorCondition(sensorCondition);
    mStackedLayout->setCurrentIndex(TransitionWidget);
  }
  else {
    mStackedLayout->setCurrentIndex(RobotWidget);
  }
}

void SwitchWidget::updateValues() {
  mViewWidget->updateValues();
}
