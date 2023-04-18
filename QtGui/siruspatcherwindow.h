#ifndef _SIRUSPATCHERWINDOW_H_
#define _SIRUSPATCHERWINDOW_H_

#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QTableWidget>
#include <QWindow>

#include "QtGui/spelldbctable.h"

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

 private slots:
  void OnChooseDirectoryButtonClicked();

 private:
  void SetupWindow();
  void SetupConnections();
  void SetupTabLabels();
  QLabel* TabLabel(const QString& text);

  Ui::SirusPatcherWindow* ui_;
  SpellDBCTable* spell_table_;
};
#endif  // _SIRUSPATCHERWINDOW_H_
