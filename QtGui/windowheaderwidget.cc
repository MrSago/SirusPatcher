#include "QtGui/windowheaderwidget.h"

#include <QMouseEvent>
#include <QPoint>
#include <QWidget>

WindowHeaderWidget::WindowHeaderWidget(QWidget* parent) : QWidget{parent} {}

void WindowHeaderWidget::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    dragging_ = true;
    drag_start_pos_ = event->globalPosition();
    window_start_pos_ = window()->pos().toPointF();
  }
}

void WindowHeaderWidget::mouseMoveEvent(QMouseEvent* event) {
  if (dragging_) {
    window()->move(
        (window_start_pos_ + event->globalPosition() - drag_start_pos_)
            .toPoint());
  }
}

void WindowHeaderWidget::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    dragging_ = false;
  }
}
