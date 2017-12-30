#include "RemoteControlWidget.hpp"
#include "EPuckRepresentation.hpp"
#include "../../remote_controls/e-puck_bluetooth/UploaderData.hpp"

#include <webots/robot.h>
#include <webots/remote_control.h>

#include <QtConcurrent/QtConcurrent>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QProgressBar>

#include <vector>

using namespace std;

RemoteControlWidget *RemoteControlWidget::cInstance = NULL;
UploaderData *RemoteControlWidget::cUploaderData = NULL;
QWidget *RemoteControlWidget::cParent = NULL;

RemoteControlWidget *RemoteControlWidget::instance() {
  if (! cInstance)
    cInstance = new RemoteControlWidget(cParent);

  return cInstance;
}

void RemoteControlWidget::clear() {
  delete cInstance;
  cInstance = NULL;
}

// constructor
RemoteControlWidget::RemoteControlWidget(QWidget *parent) :
  QWidget(parent),
  mProgressDialog(NULL),
  mPressButtonDialog(NULL),
  mConnectProgressDialog(NULL)
{
  mConnectFutureWatcher = new QFutureWatcher<int>(this);
  mUploadFutureWatcher = new QFutureWatcher<int>(this);
  mHexFileName = "";

  // create and set the combo box into this widget
  mLayout = new QGridLayout();
  mPortsComboBox = new QComboBox();
  mRefreshPortsButton = new QPushButton("Refresh ports");
  populatePorts();
  mUploadButton = new QPushButton("Upload...");
  mUploadButton->setToolTip("Upload firmware (.hex) to the e-puck robot...");
  mEnableButton = new QPushButton("Enable all");
  mEnableButton->setToolTip("Enable all the sensors");

  mLayout->addWidget(mPortsComboBox, 0, 0);
  mLayout->addWidget(mRefreshPortsButton, 0, 1);
  mLayout->addWidget(mUploadButton, 1, 0);
  mLayout->addWidget(mEnableButton, 1, 1);
  setLayout(mLayout);

  // connect the signals with their respective slots
  connect(mPortsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(portsUpdated(int)));
  connect(mRefreshPortsButton, SIGNAL(clicked()), this, SLOT(populatePorts()));
  connect(mUploadButton, SIGNAL(clicked()), this, SLOT(connectDevice()));
  connect(mEnableButton, SIGNAL(clicked()), this, SLOT(enableAllSensors()));
  connect(this, SIGNAL(progressChanged(int, int)), this, SLOT(updateProgressBar(int, int)));

  connect(mConnectFutureWatcher, SIGNAL(finished()), this, SLOT(uploadController()));
  connect(mUploadFutureWatcher, SIGNAL(finished()), this, SLOT(checkUploadFinishState()));
}

// destructor
RemoteControlWidget::~RemoteControlWidget() {
  delete mPortsComboBox;
  delete mRefreshPortsButton;
  delete mLayout;
  delete mConnectFutureWatcher;
  delete mUploadFutureWatcher;
  delete mConnectProgressDialog;
  delete mProgressDialog;
  delete mPressButtonDialog;

  delete cUploaderData;
  cUploaderData = NULL;
}

// populate the ports combo box
void RemoteControlWidget::populatePorts() {
  mPortsComboBox->blockSignals(true);
  mRefreshPortsButton->blockSignals(true);

  mPorts.clear();
  mPortsComboBox->clear();

  mPortsComboBox->addItem("Simulation");
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
    const char *port = mPortsComboBox->currentText().toUtf8().data();
    wb_robot_set_mode(WB_MODE_REMOTE_CONTROL, (void *) port);
  }
}

void RemoteControlWidget::initProgressDialogs() {
  if (! mConnectProgressDialog) {
    mConnectProgressDialog = new QProgressDialog(QString("Connecting to the e-puck robot...\t\t"), QString(), 0, 0, this);
    mConnectProgressDialog->setWindowTitle("Program upload");
    mConnectProgressDialog->setMinimum(0);
    mConnectProgressDialog->setMaximum(0);
    mConnectProgressDialog->setWindowModality(Qt::WindowModal);
  }
  mConnectProgressDialog->close();

  // create progress dialog
  if (! mProgressDialog) {
    mProgressDialog = new QProgressDialog(QString("Uploading program to the e-puck robot\t\t"), "Cancel", 0, 0, this);
    mProgressDialog->setWindowTitle("Program upload");
    mProgressDialog->setMinimum(0);
    mProgressDialog->setMaximum(100);
    mProgressDialog->setWindowModality(Qt::WindowModal);
  }
  mProgressDialog->setValue(0);
  mProgressDialog->close();

  // create button press dialog
  if (! mPressButtonDialog) {
    mPressButtonDialog = new QProgressDialog(QString("No firmware present in your e-puck robot\nPress the reset button of your e-puck robot...\t\t"), "Cancel", 0, 0, this);
    mPressButtonDialog->setWindowTitle("Program upload");
    QProgressBar *timerBar = new QProgressBar(mPressButtonDialog);
    timerBar->setFormat("%vs");
    timerBar->setMinimum(0);
    timerBar->setMaximum(10);
    timerBar->setInvertedAppearance(true);
    mPressButtonDialog->setBar(timerBar);
    mPressButtonDialog->setWindowModality(Qt::WindowModal);
  }
  mPressButtonDialog->setValue(10);
  mPressButtonDialog->close();
}

void RemoteControlWidget::cancelUpload() {
  struct UploaderData cancel;
  cancel.command = UPLOADER_DATA_CANCEL;
  callUploadCustomFunction(&cancel);
}

int RemoteControlWidget::callUploadCustomFunction(void *args) {
  // cast message received from remote control library
  return *(int *)wb_remote_control_custom_function(args);
}

void RemoteControlWidget::updateProgress(int progressType, int progressValue) {
  // emit signal handled by the main thread
  cInstance->emit progressChanged(progressType, progressValue);
}

void RemoteControlWidget::updateProgressBar(int progressType, int progressValue) {
  if (progressType == 1) {
    if (mPressButtonDialog->isVisible()) {
      // close press button dialog
      disconnect(mPressButtonDialog, SIGNAL(canceled()), this, SLOT(cancelUpload()));
      mPressButtonDialog->close();
    }

    // upload progress
    mProgressDialog->setValue(progressValue);

  } else if (progressType == 2) {
    if (!mPressButtonDialog->isVisible())
      mPressButtonDialog->setVisible(true);
    mPressButtonDialog->setValue(progressValue/10);
  }
}

void RemoteControlWidget::connectDevice() {
  // select port
  bool ok;
  static int suggestedPortIndex = mPortsComboBox->currentIndex() - 1;
  mPortName = QInputDialog::getItem(this, "Uploading program to the e-puck robot", "Choose the Bluetooth connection to your e-puck robot:\t\t", mPorts, suggestedPortIndex, false, &ok);

  if (!ok || mPortName.isEmpty())
    return;

  suggestedPortIndex = mPorts.indexOf(mPortName);
  // check if port already used for remote control
  if (mPortsComboBox->currentText() == mPortName) {
    int ret = QMessageBox::warning(this, "Port already in use",
                                   QString("Selected port '%1' is aready in use for remote control.\n"
                                            "Do you want to stop the remote control and start the upload process?"
                                          ).arg(mPortName),
                                    QMessageBox::Ok, QMessageBox::Cancel);

    if (ret == QMessageBox::Cancel)
      return;

    mPortsComboBox->setCurrentIndex(0);
  }

  initProgressDialogs();
  mConnectProgressDialog->open();

  // check connection with the given port
  delete cUploaderData;
  cUploaderData = new struct UploaderData;
  cUploaderData->command = UPLOADER_DATA_CONNECT;
  cUploaderData->data = mPortName.toUtf8().data();
  mConnectFutureWatcher->setFuture(QtConcurrent::run(this, &RemoteControlWidget::callUploadCustomFunction, (void *) cUploaderData));
}

void RemoteControlWidget::uploadController() {
  mConnectProgressDialog->close();

  if (mConnectFutureWatcher->result() != 0) {
    QMessageBox::warning(this, "Program Upload Error", QString("Failed to connect to '%1'").arg(mPortName), QMessageBox::Ok);
    return;
  }

  // select program file
#ifdef __APPLE__
  // The native file dialog returns always an empty value
  QFileDialog::Options options = QFileDialog::DontUseNativeDialog;
#else
  QFileDialog::Options options = 0;
#endif
  mHexFileName = QFileDialog::getOpenFileName(this, "Choose an HEX file to upload on the E-Puck robot", QString(), "HEX files (*.hex)", 0, options);

  if (mHexFileName.isEmpty()) {
    // close connection
    struct UploaderData disconnectData;
    disconnectData.command = UPLOADER_DATA_DISCONNECT;
    callUploadCustomFunction(&disconnectData);
    return;
  }

  // start upload of controllerFile
  connect(mProgressDialog, SIGNAL(canceled()), this, SLOT(cancelUpload()), Qt::UniqueConnection);
  connect(mPressButtonDialog, SIGNAL(canceled()), this, SLOT(cancelUpload()), Qt::UniqueConnection);
  delete cUploaderData;
  cUploaderData = new struct UploaderData;
  cUploaderData->command = UPLOADER_DATA_SEND_FILE;
  cUploaderData->data = mHexFileName.toUtf8().data();
  cUploaderData->progress_callback = RemoteControlWidget::updateProgress;
  cUploaderData->robot_id = 100;
  mUploadFutureWatcher->setFuture(QtConcurrent::run(this, &RemoteControlWidget::callUploadCustomFunction, (void *) cUploaderData));
}

void RemoteControlWidget::checkUploadFinishState() {
  disconnect(this, SLOT(cancelUpload()));
  mProgressDialog->reset();
  mPressButtonDialog->reset();

  int ret = mUploadFutureWatcher->result();
  QString fileName = QFileInfo(mHexFileName).fileName();
  if (ret == 0) {
    QMessageBox::information(this, "Program upload", QString("Upload of '%1' on the e-puck robot was successful.").arg(fileName), QMessageBox::Ok);

  } else {
    QString message;
    switch(ret) {
    case -1:
      message = QString("Failed to upload program '%1' on the e-puck robot:\n"
                        "impossible to open the file.").arg(fileName);
      break;
    case -2:
      message = QString("Failed to upload program '%1' on the e-puck robot:\n"
                        "ACK not received.").arg(fileName);
      break;
    case -3:
      message = QString("Failed to upload program '%1' on the e-puck robot:\n"
                        "checksum error.").arg(fileName);
      break;
    case -5:
      message = QString("Failed to upload program '%1' on the e-puck robot:\n"
                        "cannot open the serial port.").arg(fileName);
      break;
    case -4:
      message = QString("Failed to upload program '%1' on the e-puck robot:\n"
                        "unknown error.").arg(fileName);
      break;
    default: // -6 canceled by the user
      break;
    }

    if (!message.isEmpty())
      QMessageBox::warning(this, "Program upload", message, QMessageBox::Ok);
  }

  mHexFileName = "";
}

void RemoteControlWidget::enableAllSensors() {
  EPuckRepresentation::instance()->enableAllSensors();
  mEnableButton->setEnabled(false);
}
