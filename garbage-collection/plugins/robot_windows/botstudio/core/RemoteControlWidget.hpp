/*
 * Description:  Class defining the remote control interface group box
 */

#ifndef REMOTE_CONTROL_GROUP_BOX_HPP
#define REMOTE_CONTROL_GROUP_BOX_HPP

#include <QtWidgets/QWidget>

class QComboBox;
class QPushButton;
class QGridLayout;

class RemoteControlWidget : public QWidget
{
  Q_OBJECT

public:
  RemoteControlWidget(QWidget *parent = NULL);
  virtual ~RemoteControlWidget();
  void updateValues();

public slots:
  void portsUpdated(int index);
  void populatePorts();

private:
  QGridLayout *mLayout;
  QComboBox *mPortsComboBox;
  QPushButton *mRefreshPortsButton;
  QStringList mPorts;
};

#endif
