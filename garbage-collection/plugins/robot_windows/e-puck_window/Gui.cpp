#include "Gui.hpp"
#include "RemoteControlWidget.hpp"
#include "MainWidget.hpp"
#include "CameraGroupBox.hpp"
#include "AccelerometerGroupBox.hpp"
#include "GroundSensorGroupBox.hpp"
#include "EncoderGroupBox.hpp"
#include "EPuckRepresentation.hpp"

#include <webots/robot.h>

#include <QtWidgets/QGridLayout>

using namespace webotsQtUtils;

// constructor
Gui::Gui() :
  MainWindow()
{
  // create the widgets
  QWidget *containerWidget = new QWidget(this);
  RemoteControlWidget::setParentInstance(this);
  mRemoteControlWidget = RemoteControlWidget::instance();
  mCameraGroupBox = new CameraGroupBox(this);
  mAccelerometerGroupBox = new AccelerometerGroupBox(this);
  mEncoderGroupBox = new EncoderGroupBox(this);

  int rowsNumber = 2;
  if (EPuckRepresentation::instance()->groundSensorsExist()) {
    mGroundSensorsGroupBox = new GroundSensorGroupBox(this);
    rowsNumber++;
  } else {
    mGroundSensorsGroupBox = NULL;
  }

  mMainWidget = new MainWidget(this);

  // place them into the grid mLayout
  mLayout = new QGridLayout;
  mLayout->addWidget(mMainWidget, 0, 0, rowsNumber, 1);
  mLayout->addWidget(mRemoteControlWidget, rowsNumber, 0);
  mLayout->addWidget(mCameraGroupBox, 0, 1);
  if (mGroundSensorsGroupBox)
    mLayout->addWidget(mGroundSensorsGroupBox, 1, 1);
  mLayout->addWidget(mAccelerometerGroupBox, rowsNumber-1, 1);
  mLayout->addWidget(mEncoderGroupBox, rowsNumber, 1);
  containerWidget->setLayout(mLayout);

  setCentralWidget(containerWidget);

  // set the title
  QString title("E-puck viewer (");
  title += wb_robot_get_name();
  title += ")";
  setWindowTitle(title);

  // defining the style sheet
  setStyleSheet(
    "QGroupBox {"
    "  border: 2px groove darkGray;"
    "  border-radius: 8px;"
    "  margin-top: 1ex;"
    "}"
    "QGroupBox::title {"
    "  subcontrol-origin: margin;"
    "  subcontrol-position: top center;"
    "}"
    "QLabel#redLabel {"
    "  color: #C80000;"
    "}"
    "QLabel#greenLabel {"
    "  color: #00C800;"
    "}"
    "QLabel#blueLabel {"
    "  color: #0000C8;"
    "}"
    "QLabel#blackLabel {"
    "  color: black;"
    "}"
    "QLabel#redLabel:disabled, "
    "QLabel#greenLabel:disabled, "
    "QLabel#blueLabel:disabled, "
    "QLabel#blackLabel:disabled "
    "{"
    "  color: gray;"
    "}"
  );
}

// destructor
Gui::~Gui() {
  RemoteControlWidget::clear();
}

// update the Gui
void Gui::updateValues() {
  mMainWidget->updateValues();
  mRemoteControlWidget->updateValues();
  mCameraGroupBox->updateValues();
  mAccelerometerGroupBox->updateValues();
  mEncoderGroupBox->updateValues();
  if (mGroundSensorsGroupBox)
    mGroundSensorsGroupBox->updateValues();
}
