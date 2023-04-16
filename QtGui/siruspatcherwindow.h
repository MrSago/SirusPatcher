#ifndef _SIRUSPATCHERWINDOW_H_
#define _SIRUSPATCHERWINDOW_H_

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class SirusPatcherWindow;
}
QT_END_NAMESPACE

class SirusPatcherWindow : public QMainWindow {
  Q_OBJECT

 public:
  SirusPatcherWindow(QWidget* parent = nullptr);
  ~SirusPatcherWindow();

 private:
  Ui::SirusPatcherWindow* ui;
};
#endif  // _SIRUSPATCHERWINDOW_H_
