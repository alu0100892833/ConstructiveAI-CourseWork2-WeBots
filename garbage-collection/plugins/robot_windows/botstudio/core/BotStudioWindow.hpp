/*
 * Description:  Class defining the robot window of the e-puck
 */

#ifndef BOTSTUDIO_WINDOW_HPP
#define BOTSTUDIO_WINDOW_HPP

#include <gui/MainWindow.hpp>

class Model;
class AutomatonWidget;
class RemoteControlWidget;
class SwitchWidget;

QT_BEGIN_NAMESPACE
class QGraphicsView;
class QButtonGroup;
class QToolButton;
class QFile;
class QActionGroup;
QT_END_NAMESPACE

class BotStudioWindow : public webotsQtUtils::MainWindow
{
  Q_OBJECT

public:
  BotStudioWindow();
  virtual ~BotStudioWindow();
  void updateValues();

private slots:
  void openStateMachine();
  void saveStateMachine();
  void saveAsStateMachine();
  void selectionToolSelected(bool checked);
  void newStateToolSelected(bool checked);
  void newTransitionToolSelected(bool checked);
  void setSelectedStateAsInitial();
  void deleteSelectedObjects();
  void deleteAllObjects();
  void upload(bool);

  void updateToolBars();

  void showWarningMessage(const QString &text);

private:
  virtual void hideEvent(QHideEvent *event);

  void loadStateMachine();

  void createActions();
  void createToolBars();
  void createStatusBar();
  void createDockWindows();

  Model *mModel;
  AutomatonWidget *mAutomatonWidget;

  RemoteControlWidget *mRemoteControlWidget;
  QDockWidget *mRemoteControlDock;

  SwitchWidget *mSwitchWidget;
  QDockWidget *mSwitchWidgetDock;

  QToolBar *mFileToolBar;
  QToolBar *mEditToolBar;

  QButtonGroup *mModeButtonGroup;
  QToolButton *mSelectionButton;
  QToolButton *mNewStateButton;
  QToolButton *mNewTransitionButton;

  QActionGroup *mActionGroup;
  QAction *mNewStateMachineAct;
  QAction *mOpenStateMachineAct;
  QAction *mSaveAct;
  QAction *mSaveAsAct;
  QAction *mSelectionAct;
  QAction *mNewStateAct;
  QAction *mNewTransitionAct;
  QAction *mDeleteAct;
  QAction *mInitialStateAct;
  QAction *mUploadAct;

  QFile *mFile;

  QString mBaseWindowTitle;
};

#endif
