/*
 * Description:  Implementation of the CommonProperties.hpp functions
 */

#include "CommonProperties.hpp"

CommonProperties::CommonProperties() {
}

CommonProperties::~CommonProperties() {
}

const QColor &CommonProperties::stateColor() {
  static const QColor &stateColor = QColor(142, 191, 232);
  return stateColor;
}

const QColor &CommonProperties::transitionColor() {
  static const QColor &transitionColor = QColor(135, 230, 133);
  return transitionColor;
}

const QColor &CommonProperties::selectionColor() {
  static const QColor &selectionColor = QColor(255, 255, 102);
  return selectionColor;
}

const QColor &CommonProperties::currentStateColor() {
  static const QColor &currentStateColor = QColor(238, 33, 84);
  return currentStateColor;
}
