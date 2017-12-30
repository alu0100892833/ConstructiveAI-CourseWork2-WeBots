/*
 * Description:  Class defining the model of the state
 */

#ifndef STATE_HPP
#define STATE_HPP

#include "AutomatonObject.hpp"

#include <QtCore/QList>
#include <QtCore/QTextStream>

class Transition;
class RobotActuatorCommand;

QT_BEGIN_NAMESPACE
class QPointF;
QT_END_NAMESPACE

class State : public AutomatonObject
{
  Q_OBJECT

public:
  State(const QPointF &position);
  virtual ~State();

  bool isInitial() const { return mIsInitial; }
  bool isCurrent() const { return mIsCurrent; }
  RobotActuatorCommand *actuatorCommand() const { return mActuatorCommand; }

  void setInitial(bool isInitial);
  void setCurrent(bool isCurrent);

  void addTransition(Transition *t);
  void removeTransition(Transition *t);
  void removeTransitionAt(int i);
  void removeAllTransitions();
  QList<Transition *> transitions() const { return mTransition; }

  void fromString(const QString &string);
  void fromStringVersion3(const QString &string);  // backward compatibility code
  QString toString() const;

signals:
  void initialStateChanged();
  void currentStateChanged();

private:
  QList<Transition *> mTransition;
  bool mIsInitial;
  bool mIsCurrent;
  RobotActuatorCommand *mActuatorCommand;
};

#endif
