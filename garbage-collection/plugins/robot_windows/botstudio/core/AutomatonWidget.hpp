/*
 * Description:  Class defining the view/controller of the Automaton model
 */

#ifndef AUTOMATON_WIDGET_HPP
#define AUTOMATON_WIDGET_HPP

#include <QtWidgets/QGraphicsView>

class Model;
class State;
class AutomatonScene;

QT_BEGIN_NAMESPACE
class QPoint;
QT_END_NAMESPACE

class AutomatonWidget : public QGraphicsView
{
  Q_OBJECT

public:
  enum { SelectionMode, StateMode, TransitionMode };

  AutomatonWidget(QWidget *parent = NULL);
  virtual ~AutomatonWidget();

  void setMode(int mode);
  int mode() const { return mMode; }

  virtual QSize sizeHint() const;

signals:
  void enabledChanged(bool enabled);

private:
  virtual void wheelEvent(QWheelEvent *e);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void changeEvent(QEvent *event);

  void resetMatrix();
  void updateCursor();

  QPoint *mLastMovePos;
  int mMode;
  AutomatonScene *mAutomatonScene;
  double mZoomFactor;
  double mWheelAttenuation;
};

#endif
