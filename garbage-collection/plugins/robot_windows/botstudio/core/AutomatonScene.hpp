/*
 * Description:  Class defining the Automaton scene
 */

#ifndef AUTOMATON_SCENE_HPP
#define AUTOMATON_SCENE_HPP

#include <QtCore/QList>
#include <QtWidgets/QGraphicsScene>

class Model;
class State;
class StateRepresentation;
class Transition;
class TransitionRepresentation;
class AutomatonObject;
class AutomatonWidget;

class AutomatonScene : public QGraphicsScene
{
  Q_OBJECT

public:
  AutomatonScene(AutomatonWidget *automatonWidget);
  virtual ~AutomatonScene() {}
  StateRepresentation *findStateRepresentationFromState(State *s) const;
  TransitionRepresentation *findTransitionRepresentationFromTransition(Transition *t) const;

public slots:
  void stateCreated(State *s);
  void stateDeleted(State *s);
  void transitionCreated(Transition *s);
  void transitionDeleted(Transition *s);
  void updateEnabled(bool enabled);

private:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

  QGraphicsItem *findRepresentationFromAutomatonObject(AutomatonObject *o) const;

  Model *mModel;
  AutomatonWidget *mAutomatonWidget;
  QGraphicsLineItem *mTransitionCreationLine;
};

#endif
