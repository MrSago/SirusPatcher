#ifndef _WINDOWHEADERWIDGET_H_
#define _WINDOWHEADERWIDGET_H_

#include <QMouseEvent>
#include <QPointF>
#include <QWidget>

class WindowHeaderWidget : public QWidget {
  Q_OBJECT
 public:
  explicit WindowHeaderWidget(QWidget* parent = nullptr);

 protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

 private:
  bool dragging_;
  QPointF drag_start_pos_;
  QPointF window_start_pos_;
};

#endif  // _WINDOWHEADERWIDGET_H_
