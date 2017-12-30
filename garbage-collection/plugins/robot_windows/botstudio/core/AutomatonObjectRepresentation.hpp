/*
 * Description:  Class redefining a QGraphicsTextItem
 */

#ifndef AUTOMATON_OBJECT_REPRESENTATION_HPP
#define AUTOMATON_OBJECT_REPRESENTATION_HPP

#include <QtWidgets/QGraphicsTextItem>

class AutomatonObject;

class AutomatonObjectRepresentation : public QGraphicsTextItem
{
  Q_OBJECT

public:
  AutomatonObjectRepresentation(AutomatonObject *object);
  virtual ~AutomatonObjectRepresentation() {}

  virtual void initialize();
  QPointF computeCenter() const;
  AutomatonObject *automatonObject() const { return mAutomatonObject; }

  virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

signals:
  void selectionChanged(bool newValue);
  void positionChanged();

public slots:
  void propagateSelection(bool newValue);
  void propagatePosition();
  void propagateName();
  void emitPositionChanged();

  void updateSelection();
  void updateName();
  void updatePosition();

private:
  virtual void focusOutEvent(QFocusEvent *event);
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

  AutomatonObject *mAutomatonObject;
};

#endif
