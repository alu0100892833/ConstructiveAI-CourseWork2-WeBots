/*
 * Description:  Implementation of the EPuckLedButton.hpp functions
 */

#include "EPuckLedButton.hpp"
#include "EPuckDrawingHelper.hpp"
#include "EPuckFacade.hpp"

#include <QtWidgets/QtWidgets>

EPuckLedButton::EPuckLedButton() :
  QObject(),
  QGraphicsEllipseItem(EPuckDrawingHelper::ledRect()),
  mValue(EPuckFacade::NONE),
  mIndex(-1)
{
  updateColor();
}

void EPuckLedButton::setValue(int value) {
  mValue = value;
  updateColor();
  emit stateChanged();
}

void EPuckLedButton::updateColor() {
  switch (mValue) {
  case EPuckFacade::OFF:
    setBrush(QBrush(Qt::black));
    break;
  case EPuckFacade::ON:
    setBrush(QBrush(Qt::red));
    break;
  default:
    setBrush(QBrush(Qt::gray));
  }
}

void EPuckLedButton::mousePressEvent (QGraphicsSceneMouseEvent *event) {
  setValue((mValue+1)%3);
  QGraphicsEllipseItem::mousePressEvent(event);
}
