#ifndef _SIRUSPATCHERWINDOW_H_
#define _SIRUSPATCHERWINDOW_H_

#include <QLabel>
#include <QMainWindow>
#include <QModelIndex>
#include <QMovie>
#include <QString>
#include <QStringList>
#include <QTableWidget>
#include <QThread>
#include <QVariant>
#include <QWidget>

#include "QtGui/createpatchworker.h"
#include "QtGui/enchanttableworker.h"
#include "QtGui/mpqarchiver.h"
#include "QtGui/spelltableworker.h"

enum TabIndex { kSettingsTab = 0, kSpellTab, kEnchantTab, kAboutTab };

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
  void OnEnableAllCBButtonClicked();
  void OnDisableAllCBButtonClicked();
  void OnTabBarClicked(int index);
  void OnItemClicked(const QModelIndex& index);

  void ResetRowCountTable(QTableWidget* table, int count);
  void AddItemTable(QTableWidget* table, int row, int column,
                    const QVariant& text);

  void OnExtractingFinished();
  void OnSpellTableCreated();
  void OnEnchantTableCreated();
  void OnErrorOccurred(const QString& error);

  void OnPatchCreated();
  void AddProgressBarValue(int value);
  void ProgressBarClear();

 private:
  const QStringList kDbcFileList = {"Spell.dbc", "SpellItemEnchantment.dbc"};

  const QString kUpdatedGifPath = "://resources/icons/refresh.gif";
  const QString kCheckIconPath = "://resources/icons/check.png";
  const QString kCrossIconPath = "://resources/icons/cross.png";
  const QString kMinusIconPath = "://resources/icons/minus.png";

  void InitThreadsAndWorkers();
  void SetupWindow();
  void SetupTabLabels();
  void SetupSpellTable();
  void SetupEnchantTable();
  void SetupConnections();
  void ConnectButtons();
  void ConnectMPQArchiver();
  void ConnectSpellTable();
  void ConnectEnchantTable();
  void ConnectCreatePatch();

  QLabel* TabLabel(const QString& text);
  bool ValidateGameDirectory(QString& dir);
  void ReloadProgramState();
  void SetTableCheckBoxes(QTableWidget* table, bool state);
  QWidget* CreateCheckBox(bool state);
  void TryEnableButtons();
  void EnableButtons();
  void DisableButtons();
  void EnableTabs();
  void DisableTabs();

  Ui::SirusPatcherWindow* ui_;

  MPQArchiver* mpq_archiver_;
  SpellTableWorker* spell_table_worker_;
  EnchantTableWorker* enchant_table_worker_;
  CreatePatchWorker* create_patch_worker_;

  QThread* mpq_archiver_thread_;
  QThread* spell_table_thread_;
  QThread* enchant_table_thread_;
  QThread* create_patch_thread_;

  bool is_error_occurred_;
  int count_prepared_tables_;
  QMovie* gif_;
};

#endif  // _SIRUSPATCHERWINDOW_H_
