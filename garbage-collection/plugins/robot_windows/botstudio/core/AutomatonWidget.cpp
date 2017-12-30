/*
 * Description:  Implementation of the AutomatonWidget.hpp functions
 */

#include "AutomatonWidget.hpp"
#include "AutomatonScene.hpp"

#include <QtCore/qmath.h>
#include <QtGui/QWheelEvent>
#include <QtWidgets/QScrollBar>

AutomatonWidget::AutomatonWidget(QWidget *parent) :
    QGraphicsView(parent),
    mLastMovePos(NULL),
    mMode(SelectionMode),
    mZoomFactor(0.0),
    mWheelAttenuation(0.1)
{
  mAutomatonScene = new AutomatonScene(this);
  setRenderHint(QPainter::Antialiasing);
  setScene(mAutomatonScene);
  setSceneRect(-1000,-1000,2000,2000);
  setMode(SelectionMode);
  centerOn(0,0);
  resetMatrix();
}

QSize AutomatonWidget::sizeHint() const {
  static QSize s(500, 500);
  return s;
}

AutomatonWidget::~AutomatonWidget() {
  delete mAutomatonScene;
}

void AutomatonWidget::wheelEvent(QWheelEvent *e) {
  double direction = 1.0;
  if (e->delta() < 0)
    direction = -1.0;
  mZoomFactor += mWheelAttenuation * direction;

  resetMatrix();
}

void AutomatonWidget::mouseMoveEvent(QMouseEvent *event) {
  if (mLastMovePos) {
    QScrollBar *hBar = horizontalScrollBar();
    QScrollBar *vBar = verticalScrollBar();
    QPoint delta = event->pos() - *mLastMovePos;
    *mLastMovePos = event->pos();
    hBar->setValue(hBar->value() + (isRightToLeft() ? delta.x() : -delta.x()));
    vBar->setValue(vBar->value() - delta.y());
  }
  else
    QGraphicsView::mouseMoveEvent(event);
}

void AutomatonWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    mLastMovePos = new QPoint(event->pos());
    setCursor(Qt::ClosedHandCursor);
  }
  QGraphicsView::mousePressEvent(event);
}

void AutomatonWidget::mouseReleaseEvent(QMouseEvent *event) {
  if (mLastMovePos && event->button() == Qt::RightButton) {
    delete mLastMovePos;
    mLastMovePos = NULL;
  }
  else
    QGraphicsView::mouseReleaseEvent(event);

  updateCursor();
}

void AutomatonWidget::changeEvent(QEvent *event) {
  if (event->type() == QEvent::EnabledChange)
    emit enabledChanged(isEnabled());
  QGraphicsView::changeEvent(event);
}

void AutomatonWidget::updateCursor() {
  if (mLastMovePos)
    setCursor(Qt::ClosedHandCursor);
  else if (mMode == SelectionMode)
    setCursor(Qt::ArrowCursor);
  else
    setCursor(Qt::CrossCursor);
}

void AutomatonWidget::resetMatrix() {
  double scale = qPow(2.0, mZoomFactor);
  QMatrix matrix;
  matrix.scale(scale, scale);
  setMatrix(matrix);
}

void AutomatonWidget::setMode(int mode) {
  mMode = mode;

  if (mMode == SelectionMode)
    setDragMode(RubberBandDrag);
  else
    setDragMode(NoDrag);

  updateCursor();
}
