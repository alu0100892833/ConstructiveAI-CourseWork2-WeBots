/*
 * Description:  Class defining the widget containing the e-puck representation
 */

#ifndef MAIN_WIDGET_HPP
#define MAIN_WIDGET_HPP

#include <QtGui/QPainter>
#include <QtWidgets/QWidget>

class MainWidget : public QWidget
{
public:
  MainWidget(QWidget *parent=0);
  virtual ~MainWidget();
  virtual QSize minimumSizeHint() const;
  void updateValues();

protected:
  virtual void paintEvent(QPaintEvent *event);
  void drawCenteredText(QPainter &p, int x, int y, const QString &text);
  void drawEpuck(QPainter &p, int radius);
  void drawSpeeds(QPainter &p, int radius);
  void drawIRSensors(QPainter &p, int innerRadius, int outerRadius);
  void drawLeds(QPainter &p, int radius);
};

#endif // MAIN_WIDGET_HPP
