/*
 * Description:  Class defining some helper functions to draw the e-puck
 */

#ifndef EPUCK_DRAWING_HELPER_HPP
#define EPUCK_DRAWING_HELPER_HPP

#include <QtCore/QSize>
#include <QtCore/QRectF>

QT_BEGIN_NAMESPACE
class QGraphicsView;
class QGraphicsScene;
class QColor;
QT_END_NAMESPACE

class EPuckDrawingHelper
{
public:
  static void initQGraphicsView(QGraphicsView *view);
  static void initQGraphicsScene(QGraphicsScene *scene, const QColor &color);

  static int widgetWidth();
  static int widgetHeight();

  static int ePuckRadius();

  static double distanceSensorAngle(int i);
  static int distanceSensorRadius();
  static QSize distanceSensorSliderSize();

  static double ledAngle(int i);
  static int ledRadius();
  static QRectF ledRect();

  static int internalSliderRadius();
  static QSize internalSliderSize();

  static QSize groundSensorSliderSize();
  static int groundSensorSliderDistance();
  static int groundSensorSliderRadius();

private:
  static void drawEPuck(QGraphicsScene *scene);

  EPuckDrawingHelper();
  ~EPuckDrawingHelper();
};

#endif
