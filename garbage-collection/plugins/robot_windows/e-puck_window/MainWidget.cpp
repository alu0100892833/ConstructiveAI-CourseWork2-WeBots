#include "MainWidget.hpp"
#include "EPuckRepresentation.hpp"

#include <webots/robot.h>
#include <webots/device.h>
#include <webots/distance_sensor.h>
#include <webots/light_sensor.h>
#include <webots/led.h>

#include <QtWidgets/QtWidgets>

#include <cmath>

// direction of the IR Devices and of the Leds from the center along the xz plane
// these angles are given in radians and starting from the right of the robot
static double angleIRDevices[NUMBER_OF_DISTANCE_SENSORS] = {
  M_PI_2 - 15.0*M_PI/180.0,
  M_PI_2 - 50.0*M_PI/180.0,
  0.0,
  3*M_PI_2 + 25.0*M_PI/180.0,
  3*M_PI_2 - 25.0*M_PI/180.0,
  M_PI,
  M_PI_2 + 50.0*M_PI/180.0,
  M_PI_2 + 15.0*M_PI/180.0
};
static double angleLed[NUMBER_OF_LEDS] = {
  M_PI_2,
  M_PI_2 - 60.0*M_PI/180.0,
  0.0,
  3*M_PI_2 + 30.0*M_PI/180.0,
  3*M_PI_2,
  3*M_PI_2 - 30.0*M_PI/180.0,
  M_PI,
  M_PI_2 + 60.0*M_PI/180.0,
  0.0,                        // body led - not used
  M_PI_2 - 15.0*M_PI/180.0    // front led
};

// predetermined colors (extending the colors of the Qt namespace)
static QColor red    (200,   0,   0);
static QColor green  (  0, 210,   0);
static QColor blue   (  0,   0, 200);
static QColor yellow (200, 200,   0);

// constructor
MainWidget::MainWidget(QWidget *parent):
  QWidget(parent)
{
  // take as surface as possible
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

// destructor
MainWidget::~MainWidget() {
  // nothing to destroy
}

// Set the minimum size of this widget
QSize MainWidget::minimumSizeHint() const {
  static QSize s(180, 180);
  return s;
}

// draw the widget
void MainWidget::paintEvent(QPaintEvent *event) {

  // compute stuff
  int w = width();
  int h = height();
  int ref = (w>h)?h:w; // ref = minimum between w and h
  int irDeviceRadius = ref*60/100/2; // 60% of the half of the ref
  int epuckRadius    = ref*65/100/2; // 65% of the half of the ref
  int labelRadius    = ref*87/100/2; // 87% of the half of the ref

  // create the painter
  QPainter painter(this);

  // draw the e-puck
  drawEpuck(painter, epuckRadius);

  // draw the squares representing the ir devices, draw the distance sensor values,
  // and draw the light sensor values
  drawIRSensors(painter, irDeviceRadius, labelRadius);

  // draw the leds
  drawLeds(painter, epuckRadius);

  // draw the speed bars
  drawSpeeds(painter, epuckRadius);
}

// draw the squares representing the ir devices, draw the distance sensor values,
// and draw the light sensor values
void MainWidget::drawIRSensors(QPainter &p, int innerRadius, int outerRadius) {
  int w = width();
  int h = height();
  int squareSize = 5;
  QFontMetrics metrics(font());
  int fontHeight_2 = metrics.height()/2;
  EPuckRepresentation *epuck = EPuckRepresentation::instance();

  p.setBrush(QBrush(Qt::gray));
  for (int i=0; i<8; i++) {
    // compute the coordinates
    double uI = w/2 + innerRadius*cos(angleIRDevices[i]);
    double vI = h/2 - innerRadius*sin(angleIRDevices[i]);
    double uO = w/2 + outerRadius*cos(angleIRDevices[i]);
    double vO = h/2 - outerRadius*sin(angleIRDevices[i]);
    int offset_ds = fontHeight_2;
    int offset_ls = -fontHeight_2;
    bool dsEnabled = epuck->isDistanceSensorEnabled(i);
    bool lsEnabled = epuck->isLightSensorEnabled(i);

    if ((dsEnabled && !lsEnabled) || (!dsEnabled && lsEnabled)) {
      offset_ds = 0;
      offset_ls = 0;
    }

    // draw the ir representation
    p.setPen(QPen(Qt::black));
    p.save();
    p.translate(uI, vI);
    p.rotate(-angleIRDevices[i]*180.0/M_PI);
    p.drawRect(- squareSize/2, - squareSize, squareSize, 2*squareSize);
    p.restore();

    // draw the values of the distance sensors if needed
    if (dsEnabled) {
      p.setPen(QPen(red));
      float ds = epuck->distanceSensorValue(i);
      drawCenteredText(p, uO, vO + offset_ds, QString::number(ds, 'f', 0));
    }

    // draw the values of the light sensors if needed
    if (lsEnabled) {
      p.setPen(QPen(yellow));
      float ls = epuck->lightSensorValue(i);
      drawCenteredText(p, uO, vO + offset_ls, QString::number(ls, 'f', 0));
    }
  }
}

// draw the 8 leds + the front led
void MainWidget::drawLeds(QPainter &p, int radius) {
  int w = width();
  int h = height();
  int ledSize = 8;
  EPuckRepresentation *epuck = EPuckRepresentation::instance();

  p.setPen(QPen(Qt::black));
  // draw the 8 leds + the front led
  for (int i=0; i<10; i++) {
    if (i==8) // the body led is not drawn in this function
      continue;
    int u = w/2 + radius*cos(angleLed[i]);
    int v = h/2 - radius*sin(angleLed[i]);

    // set the right brush according to the led state
    if (epuck->ledValue(i))
      p.setBrush(QBrush(Qt::red));
    else
      p.setBrush(QBrush(Qt::black));

    // draw the led
    p.save();
    p.translate(u, v);
    if (i==9) // the front led is a circle
      p.drawEllipse(-ledSize/2, -ledSize/2, ledSize, ledSize);
    else { // while the others are a rotated rectangle
      p.rotate(-angleLed[i]*180.0/M_PI);
      p.drawRect(-ledSize/2, -ledSize/2, ledSize, ledSize);
    }
    p.restore();
  }
}

// draw the speed sliders
void MainWidget::drawSpeeds(QPainter &p, int radius) {
  int w = width();
  int h = height();
  QFontMetrics metrics(font());
  EPuckRepresentation *epuck = EPuckRepresentation::instance();
  int speeds[2] = { static_cast<int>(epuck->leftSpeed()), static_cast<int>(epuck->rightSpeed()) };
  int barWidth = 10;
  int barHeight = 0.75 * (2.0*0.7071*radius);
  int sliderWidth = 14;
  int sliderHeight = 4;
  int worstTextWidth_2 = metrics.tightBoundingRect("-1000").width() / 2;

  // draw the two sliders
  for (int i=0; i<2; i++) {
    int sign = (i==0)?-1:1;
    int centerX = w/2 + sign*0.7071*radius;
    int centerY = h/2;

    // draw the bar
    p.setPen(QPen(Qt::black));
    p.setBrush(QBrush(Qt::lightGray));
    p.drawRect(centerX - barWidth/2, centerY - barHeight/2, barWidth, barHeight);

    // draw the slider
    centerY -= speeds[i] * barHeight / 1000 / 2;
    p.setBrush(QBrush(blue));
    p.drawRect(centerX - sliderWidth/2, centerY - sliderHeight/2, sliderWidth, sliderHeight);

    // draw the text
    p.setPen(QPen(blue));
    centerX = w/2 + sign*0.7071*radius - sign*(worstTextWidth_2 + sliderWidth/2 + 1);
    drawCenteredText(p, centerX, centerY, QString::number(speeds[i]));
  }
}

// draw an e-puck at the center of the widget
// if the body led is enabled, draw the circle in green
void MainWidget::drawEpuck(QPainter &p, int radius) {
  int w = width();
  int h = height();
  EPuckRepresentation *epuck = EPuckRepresentation::instance();

  p.setPen(QPen(Qt::black));
  if (epuck->ledValue(8))
    p.setBrush(QBrush(green));
  else
    p.setBrush(QBrush(Qt::lightGray));
  p.drawEllipse (w/2-radius, h/2-radius, 2*radius, 2*radius);
}

// draw a text centered around the x,y point (instead of bottom-left)
void MainWidget::drawCenteredText(QPainter &p, int x, int y, const QString &text) {
  QFontMetrics metrics(font());
  QRect textBounds = metrics.tightBoundingRect(text);
  double u = -0.5*textBounds.width();
  double v =  0.5*textBounds.height();
  p.drawText(x+u, y+v, text);
}

void MainWidget::updateValues() {
  update();
}
