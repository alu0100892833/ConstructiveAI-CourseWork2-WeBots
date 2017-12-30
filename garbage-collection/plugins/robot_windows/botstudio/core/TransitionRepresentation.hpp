/*
 * Description:  Class defining the graphical representation of a transition
 */

#ifndef TRANSITION_REPRESENTATION_HPP
#define TRANSITION_REPRESENTATION_HPP

#include "AutomatonObjectRepresentation.hpp"

class AutomatonScene;
class StateRepresentation;
class Transition;

QT_BEGIN_NAMESPACE
class QGraphicsPathItem;
QT_END_NAMESPACE

class TransitionRepresentation : public AutomatonObjectRepresentation
{
  Q_OBJECT

public:
  TransitionRepresentation(Transition *t);
  virtual ~TransitionRepresentation();
  virtual void initialize();
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  Transition *transition() const;

public slots:
  void updatePathItem();

private:
  QGraphicsPathItem *mPathItem;

  AutomatonScene *mAutomatonScene;
  StateRepresentation *mStartState;
  StateRepresentation *mEndState;

  bool mIsInitialized;
};

#endif
