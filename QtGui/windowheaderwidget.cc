#include "windowheaderwidget.h"

#include <QMouseEvent>
#include <QWidget>
#include <QWindow>

WindowHeaderWidget::WindowHeaderWidget(QWidget* parent) : QWidget{parent} {}

void WindowHeaderWidget::mousePressEvent(QMouseEvent* event) {
  start_pos_ = event->pos();
  QWidget::mousePressEvent(event);
}

void WindowHeaderWidget::mouseMoveEvent(QMouseEvent* event) {
  QPoint delta = event->pos() - start_pos_;
  QWidget* w = this->window();
  if (w) w->move(w->pos() + delta);
  QWidget::mouseMoveEvent(event);
}
