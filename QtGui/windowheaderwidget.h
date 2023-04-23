#ifndef _WINDOWHEADERWIDGET_H_
#define _WINDOWHEADERWIDGET_H_

#include <QMouseEvent>
#include <QPoint>
#include <QWidget>

class WindowHeaderWidget : public QWidget {
  Q_OBJECT
 public:
  explicit WindowHeaderWidget(QWidget* parent = nullptr);

 protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

 private:
  QPoint start_pos_;
};

#endif  // _WINDOWHEADERWIDGET_H_
