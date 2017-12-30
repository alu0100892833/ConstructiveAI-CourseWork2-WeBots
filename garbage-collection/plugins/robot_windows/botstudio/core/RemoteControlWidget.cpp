/*
 * Description:  Implementation of the RemoteControlWidget.hpp functions
 */

#include "RemoteControlWidget.hpp"

#include <webots/robot.h>
#include <webots/remote_control.h>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGridLayout>

#include <vector>

using namespace std;

// constructor
RemoteControlWidget::RemoteControlWidget(QWidget *parent) :
    QWidget(parent)
{
  // create and set the combo box into this widget
  mLayout = new QGridLayout();
  mPortsComboBox = new QComboBox();
  mRefreshPortsButton = new QPushButton(tr("Refresh ports"));
  populatePorts();
  mLayout->addWidget(mPortsComboBox, 0, 0);
  mLayout->addWidget(mRefreshPortsButton, 0, 1);
  setLayout(mLayout);

  // connect the signals with their respective slots
  connect(mPortsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(portsUpdated(int)));
  connect(mRefreshPortsButton, SIGNAL(clicked()), this, SLOT(populatePorts()));
}

// destructor
RemoteControlWidget::~RemoteControlWidget() {
  delete mPortsComboBox;
  delete mRefreshPortsButton;
  delete mLayout;
}

// populate the ports combo box
void RemoteControlWidget::populatePorts() {
  mPortsComboBox->blockSignals(true);
  mRefreshPortsButton->blockSignals(true);

  mPorts.clear();
  mPortsComboBox->clear();

  mPortsComboBox->addItem(tr("Simulation"));
  mPortsComboBox->setCurrentIndex(0);

  char *p = (char *)wb_remote_control_custom_function(NULL);
  QStringList ports = QString::fromUtf8(p).split('\n');
  free(p);
  for(int i = 0; i < ports.size(); ++i) {
    mPorts << ports[i];
    mPortsComboBox->addItem(ports[i]);
  }

  mPortsComboBox->blockSignals(false);
  mRefreshPortsButton->blockSignals(false);
}

// update
void RemoteControlWidget::updateValues() {
  // check that the remote control is still active
  if (wb_robot_get_mode() == 0 && mPortsComboBox->currentIndex() != 0)
    mPortsComboBox->setCurrentIndex(0);
}

// handle the combo box selection
void RemoteControlWidget::portsUpdated(int index) {
  if (index == 0)
    wb_robot_set_mode(WB_MODE_SIMULATION, NULL);
  else {
    const string &port = mPortsComboBox->currentText().toStdString();
    wb_robot_set_mode(WB_MODE_REMOTE_CONTROL, (void *) &port);
  }
}
