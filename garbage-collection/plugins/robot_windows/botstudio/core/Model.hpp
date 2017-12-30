/*
 * Description:  Class defining the internal model of BotStudio
 */

#ifndef MODEL_HPP
#define MODEL_HPP

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

class Automaton;
class RobotFacade;

class Model : public QObject
{
  Q_OBJECT

public:
  static Model *instance();
  static void killInstance();

  Automaton *automaton() const { return mAutomaton; }
  RobotFacade *robotFacade() const { return mRobotFacade; }
  bool isRunning() const { return mIsRunning; }

  void setRunning(bool running);
  void updateValues();

  void fromString(const QString &string);
  QString toString() const;

private:
  Model();
  virtual ~Model();

  int macroVersion() const { return 4; }
  int microVersion() const { return 0; }

  static Model *cInstance;

  Automaton *mAutomaton;
  RobotFacade *mRobotFacade;
  bool mIsRunning;
};

#endif
