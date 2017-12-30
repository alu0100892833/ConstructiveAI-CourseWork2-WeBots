/*
 * Description:  Class defining a led button
 */

#ifndef EPUCKmLed_BUTTON_HPP
#define EPUCKmLed_BUTTON_HPP

#include <QtCore/QObject>
#include <QtWidgets/QGraphicsEllipseItem>

class EPuckLedButton : public QObject, public QGraphicsEllipseItem
{
  Q_OBJECT

public:

  EPuckLedButton();
  virtual ~EPuckLedButton() {}

  void setIndex(int index) { mIndex = index; }
  void setValue(int value);

  int index() const { return mIndex; }
  int value() const { return mValue; }

  virtual void mousePressEvent (QGraphicsSceneMouseEvent * event);

signals:
  void stateChanged();

protected:
  void updateColor();

  int mValue;
  int mIndex;
};

#endif
