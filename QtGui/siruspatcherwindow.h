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

enum TabIndex {
  kSettingsTab = 0,
  kSpellTab,
  kEnchantTab,
  kFeaturesTab,
  kAboutTab
};

QT_BEGIN_NAMESPACE
namespace Ui {
class SirusPatcherWindow;
}
QT_END_NAMESPACE

class SirusPatcherWindow : public QMainWindow {
  Q_OBJECT

 private:
  static const QStringList kDbcFileList;

  static const QString kRefreshGifPath;
  static const QString kCheckIconPath;
  static const QString kCrossIconPath;
  static const QString kMinusIconPath;

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

  void OnExtractingFinished();
  void OnSpellTableCreated();
  void OnEnchantTableCreated();
  void OnErrorOccurred(const QString& error);
  void OnWarningOccurred(const QString& warning);
  void OnPatchCreated();

  void SetRowCountTable(QTableWidget* table, int count);
  void ResetRowCountTable(QTableWidget* table, int count);
  void AddItemTable(QTableWidget* table, int row, int column,
                    const QVariant& text);

  void AddProgressBarValue(int value);
  void SetProgressBarMaximum(int maximum);
  void ProgressBarClear();
  void FillProgressBarToMax();
  void AddTotalRecords(int count);

 private:
  void InitThreadsAndWorkers();
  void InitTabs();
  void InitSpellTable();
  void InitEnchantTable();
  void InitEventHandlers();
  void ConnectButtons();
  void ConnectMPQArchiver();
  void ConnectSpellTable();
  void ConnectEnchantTable();
  void ConnectCreatePatch();

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

  bool is_error_occurred_ = false;
  int count_prepared_tables_ = 0;
  int total_records_count_ = 0;
  QMovie* gif_ = nullptr;
};

#endif  // _SIRUSPATCHERWINDOW_H_
