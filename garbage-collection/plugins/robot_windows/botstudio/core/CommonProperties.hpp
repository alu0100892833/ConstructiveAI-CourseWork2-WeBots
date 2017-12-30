/*
 * Description:  Class defining the interclasses common properties
 */

#ifndef COMMON_PROPERTIES_HPP
#define COMMON_PROPERTIES_HPP

#include <QtGui/QColor>

class CommonProperties
{
public:
  static const QColor &stateColor();
  static const QColor &transitionColor();
  static const QColor &selectionColor();
  static const QColor &currentStateColor();

private:
  CommonProperties();
  ~CommonProperties();
};

#endif
