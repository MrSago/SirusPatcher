#ifndef _SIRUSPATCHERWINDOW_H_
#define _SIRUSPATCHERWINDOW_H_

#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QTableWidget>
#include <QWindow>

#include "QtGui/abstractdbctable.h"
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

 public slots:
  void OnChooseDirectoryButtonClicked();
  void OnCreatePatchButtonClicked();
  void OnEnableAllCBSPellButtonClicked();
  void OnDisableAllCBSpellButtonClicked();
  void ProgressBarClear();

 private:
  const QStringList kDbcFileList = {"Spell.dbc" /*Another.dbc*/};

  void SetupWindow();
  void SetupConnections();
  void SetupTabLabels();
  QLabel* TabLabel(const QString& text);
  bool ValidateGameDirectory(QString& dir);
  bool SetupTables();
  void ReloadProgramState();
  void SetTableCheckBoxes(QTableWidget* table, Qt::CheckState state);

  Ui::SirusPatcherWindow* ui_;
  AbstractDBCTable* spell_table_;
};
#endif  // _SIRUSPATCHERWINDOW_H_
