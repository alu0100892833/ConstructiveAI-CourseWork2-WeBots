/*
 * Description:   Class defining the remote control interface group box
 */

#ifndef REMOTE_CONTROL_GROUP_BOX_HPP
#define REMOTE_CONTROL_GROUP_BOX_HPP

#include <QtCore/QFutureWatcher>
#include <QtWidgets/QWidget>

class QComboBox;
class QPushButton;
class QGridLayout;
class QProgressDialog;
struct UploaderData;

class RemoteControlWidget : public QWidget
{
  Q_OBJECT

public:
  static RemoteControlWidget *instance();
  static void clear();
  static void setParentInstance(QWidget *parent) { cParent = parent; }
  void updateValues();

public slots:
  void portsUpdated(int index);
  void populatePorts();
  void connectDevice();
  void uploadController();
  void enableAllSensors();

signals:
  void progressChanged(int type, int newValue);

private:
  static RemoteControlWidget *cInstance;
  static QWidget *cParent;

  RemoteControlWidget(QWidget *parent = NULL);
  ~RemoteControlWidget();

  QGridLayout *mLayout;
  QComboBox *mPortsComboBox;
  QPushButton *mRefreshPortsButton;
  QPushButton *mUploadButton;
  QPushButton *mEnableButton;
  QStringList mPorts;
  QString mPortName;
  QString mHexFileName;
  QProgressDialog *mProgressDialog;
  QProgressDialog *mPressButtonDialog;
  QProgressDialog *mConnectProgressDialog;
  QFutureWatcher<int> *mConnectFutureWatcher;
  QFutureWatcher<int> *mUploadFutureWatcher;

  void initProgressDialogs();
  int callUploadCustomFunction(void *args);
  void checkUploadFinishState(int ret);

  static UploaderData *cUploaderData;
  static void updateProgress(int progressType, int progressValue);

private slots:
  void checkUploadFinishState(void);
  void updateProgressBar(int progressType, int progressValue);
  void cancelUpload(void);
};

#endif
