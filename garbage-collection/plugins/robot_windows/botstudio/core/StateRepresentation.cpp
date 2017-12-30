/*
 * Description:  Implementation of the StateRepresentation.hpp functions
 */

#include "StateRepresentation.hpp"
#include "State.hpp"
#include "CommonProperties.hpp"

#include <QtGui/QPainter>
#include <QtWidgets/QStyle>
#include <QtWidgets/QWidget>

StateRepresentation::StateRepresentation(State *s) :
    AutomatonObjectRepresentation(s)
{
  setZValue(1.0);

  QFont f = font();
  f.setPointSize(10);
  f.setBold(true);
  setFont(f);
}

void StateRepresentation::initialize() {
  AutomatonObjectRepresentation::initialize();
  connect(state(), SIGNAL(initialStateChanged()), this, SLOT(callUpdate()));
  connect(state(), SIGNAL(currentStateChanged()), this, SLOT(callUpdate()));
}

void StateRepresentation::callUpdate() {
  update();
}

void StateRepresentation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  painter->setPen(Qt::black);
  QPainterPath path;

  QColor color = CommonProperties::stateColor();
  if (isSelected())
    color = CommonProperties::selectionColor();
  if (!isEnabled()) {
    if (state()->isCurrent())
      color = CommonProperties::currentStateColor();
    else
      color = widget->style()->standardPalette().color(QPalette::Window);
  }

  QRectF br = boundingRect();
  if (state()->isInitial()) {
    path.addRect(br);
    painter->fillPath(path, QBrush(color.lighter(125)));
    painter->drawPath(path);
    path = QPainterPath();
    br.setSize(br.size() - QSizeF(3.0, 3.0));
    br.setTopLeft(br.topLeft() + QPointF(3.0, 3.0));
  }
  path.addRect(br);

  painter->fillPath(path, QBrush(color));
  painter->drawPath(path);

  AutomatonObjectRepresentation::paint(painter, option, widget);
}

State *StateRepresentation::state() const {
  return static_cast<State *>(automatonObject());
}
