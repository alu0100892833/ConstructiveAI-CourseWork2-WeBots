/*
 * Description:  Implementation of the AutomatonObjectRepresentation.hpp functions
 */

#include "AutomatonObject.hpp"
#include "AutomatonObjectRepresentation.hpp"

#include <QtWidgets/QtWidgets>

AutomatonObjectRepresentation::AutomatonObjectRepresentation(AutomatonObject *object) :
  QGraphicsTextItem(object->name()),
  mAutomatonObject(object)
{
  setFlag(QGraphicsItem::ItemIsMovable);
  setFlag(QGraphicsItem::ItemIsSelectable);

  setPos(mAutomatonObject->position());
  setSelected(mAutomatonObject->isSelected());
}

// init should be called after the attachment of the object over the scene
void AutomatonObjectRepresentation::initialize() {
  connect(this, SIGNAL(xChanged()), this, SLOT(emitPositionChanged()));
  connect(this, SIGNAL(yChanged()), this, SLOT(emitPositionChanged()));

  connect(this, SIGNAL(positionChanged()), this, SLOT(propagatePosition()));
  connect(this, SIGNAL(selectionChanged(bool)), this, SLOT(propagateSelection(bool)));
  connect(document(), SIGNAL(contentsChanged()), this, SLOT(propagateName()));

  connect(mAutomatonObject, SIGNAL(nameUpdated()), this, SLOT(updateName()));
  connect(mAutomatonObject, SIGNAL(selectionChanged()), this, SLOT(updateSelection()));
  connect(mAutomatonObject, SIGNAL(positionUpdated()), this, SLOT(updatePosition()));
}

void AutomatonObjectRepresentation::updateName() {
  if (mAutomatonObject->name() != document()->toPlainText())
    document()->setPlainText(mAutomatonObject->name());
}

void AutomatonObjectRepresentation::updateSelection() {
  if (mAutomatonObject->isSelected() != isSelected())
    setSelected(mAutomatonObject->isSelected());
}

void AutomatonObjectRepresentation::updatePosition() {
  if (mAutomatonObject->position() != pos())
    setPos(mAutomatonObject->position());
}

void AutomatonObjectRepresentation::emitPositionChanged() {
  emit positionChanged();
}

QPointF AutomatonObjectRepresentation::computeCenter() const {
  return pos() + boundingRect().center();
}

void AutomatonObjectRepresentation::focusOutEvent(QFocusEvent *event) {
  setTextInteractionFlags(Qt::NoTextInteraction);
  QGraphicsTextItem::focusOutEvent(event);
}

void AutomatonObjectRepresentation::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  if (textInteractionFlags() == Qt::NoTextInteraction) {
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setFocus();
  }
  else
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

QVariant AutomatonObjectRepresentation::itemChange(GraphicsItemChange change, const QVariant &value) {
   if (change == ItemSelectedChange)
     emit selectionChanged(value.toBool());
   return QGraphicsItem::itemChange(change, value);
}

void AutomatonObjectRepresentation::propagatePosition() {
  if (mAutomatonObject->position() != pos())
    mAutomatonObject->setPosition(pos());
}

void AutomatonObjectRepresentation::propagateName() {
  if (mAutomatonObject->name() != document()->toPlainText())
    mAutomatonObject->setName(document()->toPlainText());
}

void AutomatonObjectRepresentation::propagateSelection(bool newValue) {
  if (mAutomatonObject->isSelected() != newValue)
    mAutomatonObject->setSelected(newValue);
}
