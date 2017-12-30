/*
 * Description:  Class defining the model of a transition
 */

#ifndef TRANSITION_HPP
#define TRANSITION_HPP

#include "AutomatonObject.hpp"
#include <QtCore/QTextStream>

class Automaton;
class State;
class RobotSensorCondition;

QT_BEGIN_NAMESPACE
class QPointF;
QT_END_NAMESPACE

class Transition : public AutomatonObject
{
public:
  Transition(const QPointF &position, State *startState, State *endState);
  virtual ~Transition();

  void setStartState(State *startState) { mStartState = startState; }
  void setEndState(State *endState) { mEndState = endState; }

  State *startState() const { return mStartState; }
  State *endState() const { return mEndState; }
  RobotSensorCondition *sensorCondition() const { return mSensorCondition; }

  void fromString(const QString &string);
  void fromStringVersion3(const QString &string);  // backward compatibility code
  QString toString() const;

private:
  State *mStartState;
  State *mEndState;
  RobotSensorCondition *mSensorCondition;
};

#endif
