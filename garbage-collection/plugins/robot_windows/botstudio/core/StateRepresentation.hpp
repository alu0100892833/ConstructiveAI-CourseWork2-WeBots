/*
 * Description:  Class defining the graphical representation of a state
 */

#ifndef STATE_REPRESENTATION_HPP
#define STATE_REPRESENTATION_HPP

#include "AutomatonObjectRepresentation.hpp"

class State;

class StateRepresentation : public AutomatonObjectRepresentation
{
  Q_OBJECT

public:
  StateRepresentation(State *s);
  virtual ~StateRepresentation() {}
  virtual void initialize();
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  State *state() const;

private slots:
  void callUpdate();
};

#endif
