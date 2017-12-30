/*
 * Description:  Implementation of the AutomatonObject.hpp functions
 */

#include "AutomatonObject.hpp"

int AutomatonObject::cCurrentGlobalId = 0;

AutomatonObject::AutomatonObject(const QPointF &position) :
    QObject(),
    mPosition(position),
    mIsSelected(false)
{
  mUniqueId = cCurrentGlobalId++;
  mName = QString::number(mUniqueId);
  setObjectName(mName);
}

void AutomatonObject::setName(const QString &name) {
  mName = name;
  emit nameUpdated();
}

void AutomatonObject::setSelected(bool isSelected) {
  mIsSelected = isSelected;
  emit selectionChanged();
}

void AutomatonObject::setPosition(const QPointF &position) {
  mPosition = position;
  emit positionUpdated();
}

void AutomatonObject::computeCurrentGlobalId(int maxId) {
  if (cCurrentGlobalId <= maxId)
    cCurrentGlobalId = maxId + 1;
}
