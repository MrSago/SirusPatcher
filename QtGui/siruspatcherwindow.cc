#include "QtGui/siruspatcherwindow.h"

#include <QAbstractItemView>
#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QModelIndex>
#include <QMouseEvent>
#include <QMovie>
#include <QPixmap>
#include <QPushButton>
#include <QTabBar>
#include <QTableWidget>
#include <QThread>
#include <QVariant>
#include <QWidget>

#include "./ui_siruspatcherwindow.h"
#include "QtGui/createpatchworker.h"
#include "QtGui/enchanttableworker.h"
#include "QtGui/jsontablehandler.h"
#include "QtGui/mpqarchiver.h"
#include "QtGui/proginfogetter.h"
#include "QtGui/spelltableworker.h"

const QStringList SirusPatcherWindow::kDbcFileList = {
    "Spell.dbc",
    "SpellItemEnchantment.dbc",
    "SpellVisualEffectName.dbc",
    "CreatureDisplayInfo.dbc",
};

const QString SirusPatcherWindow::kRefreshGifPath =
    "://resources/icons/refresh.gif";
const QString SirusPatcherWindow::kCheckIconPath =
    "://resources/icons/check.png";
const QString SirusPatcherWindow::kCrossIconPath =
    "://resources/icons/cross.png";
const QString SirusPatcherWindow::kMinusIconPath =
    "://resources/icons/minus.png";

const QString SirusPatcherWindow::kJsonTablePath = "./settings/";
const QString SirusPatcherWindow::kJsonSpellTablePath =
    SirusPatcherWindow::kJsonTablePath + "spell.json";
const QString SirusPatcherWindow::kJsonEnchantTablePath =
    SirusPatcherWindow::kJsonTablePath + "enchant.json";

SirusPatcherWindow::SirusPatcherWindow(QWidget* parent)
    : QMainWindow(parent), ui_(new Ui::SirusPatcherWindow) {
  ui_->setupUi(this);
  spell_json_table_ = new JsonTableHandler(ui_->SpellTableWidget),
  enchant_json_table_ = new JsonTableHandler(ui_->EnchantTableWidget),
  InitThreadsAndWorkers();
  InitTabs();
  InitSpellTable();
  InitEnchantTable();
  InitEventHandlers();
}

SirusPatcherWindow::~SirusPatcherWindow() {
  mpq_archiver_thread_->quit();
  mpq_archiver_thread_->wait();
  delete mpq_archiver_thread_;
  delete mpq_archiver_;

  spell_table_thread_->quit();
  spell_table_thread_->wait();
  delete spell_table_thread_;
  delete spell_table_worker_;

  enchant_table_thread_->quit();
  enchant_table_thread_->wait();
  delete enchant_table_thread_;
  delete enchant_table_worker_;

  create_patch_thread_->quit();
  create_patch_thread_->wait();
  delete create_patch_thread_;
  delete create_patch_worker_;

  if (gif_) delete gif_;

  delete ui_;
}

#pragma region Slots

void SirusPatcherWindow::OnChooseDirectoryButtonClicked() {
  ProgressBarClear();
  SetProgressBarMaximum(0);

  gif_ = new QMovie(kRefreshGifPath);
  gif_->start();
  ui_->DirectoryCorrectStatus->setMovie(gif_);

  QString started_dir;
  if (QDir(ui_->GameDirectoryLine->text()).exists()) {
    started_dir = ui_->GameDirectoryLine->text();
  } else {
    started_dir = QDir::currentPath();
  }

  QString game_dir = QFileDialog::getExistingDirectory(
      this, "Выбор директории с игрой", started_dir, QFileDialog::ShowDirsOnly);
  if (!ValidateGameDirectory(game_dir)) {
    ui_->DirectoryCorrectStatus->setPixmap(QPixmap(
        started_dir != QDir::currentPath() ? kCheckIconPath : kCrossIconPath));
    SetProgressBarMaximum(1);
    delete gif_, gif_ = nullptr;
    return;
  }
  ui_->DirectoryCorrectStatus->setPixmap(QPixmap(kCheckIconPath));
  ui_->GameDirectoryLine->setText(game_dir);

  ReloadProgramState();

  ui_->MpqUnpackedStatus->setMovie(gif_);
  mpq_archiver_->SetParameters(kDbcFileList, game_dir, QDir::currentPath());
  mpq_archiver_thread_->start();
  mpq_archiver_thread_->quit();
}

void SirusPatcherWindow::OnCreatePatchButtonClicked() {
  ProgressBarClear();
  DisableTabs();
  DisableButtons();

  create_patch_thread_->start();
  create_patch_thread_->quit();

  spell_json_table_->SaveJsonTable(kJsonTablePath, kJsonSpellTablePath);
  enchant_json_table_->SaveJsonTable(kJsonTablePath, kJsonEnchantTablePath);
}

void SirusPatcherWindow::OnEnableAllCBButtonClicked() {
  switch (ui_->MainTabWidget->currentIndex()) {
    case kSpellTab:
      SetTableCheckBoxes(ui_->SpellTableWidget, true);
      break;
    case kEnchantTab:
      SetTableCheckBoxes(ui_->EnchantTableWidget, true);
      break;
    default:
      break;
  }
}

void SirusPatcherWindow::OnDisableAllCBButtonClicked() {
  switch (ui_->MainTabWidget->currentIndex()) {
    case kSpellTab:
      SetTableCheckBoxes(ui_->SpellTableWidget, false);
      break;
    case kEnchantTab:
      SetTableCheckBoxes(ui_->EnchantTableWidget, false);
      break;
    default:
      break;
  }
}

void SirusPatcherWindow::OnTabBarClicked(int index) {
  ProgressBarClear();

  switch (index) {
    case kSpellTab:
    case kEnchantTab:
      ui_->EnableAllCBButton->setEnabled(true);
      ui_->DisableAllCBButton->setEnabled(true);
      break;
    default:
      ui_->EnableAllCBButton->setEnabled(false);
      ui_->DisableAllCBButton->setEnabled(false);
      break;
  }
}

void SirusPatcherWindow::OnItemClicked(const QModelIndex& index) {
  QTableWidget* table;
  switch (ui_->MainTabWidget->currentIndex()) {
    case kSpellTab:
      table = ui_->SpellTableWidget;
      break;
    case kEnchantTab:
      table = ui_->EnchantTableWidget;
      break;
    default:
      return;
  }

  int rowIndex = index.row();
  QCheckBox* check_box = qobject_cast<QCheckBox*>(
      table->cellWidget(rowIndex, 0)->layout()->itemAt(0)->widget());
  check_box->setChecked(!check_box->isChecked());
}

void SirusPatcherWindow::OnExtractingFinished() {
  ui_->MpqUnpackedStatus->setPixmap(QPixmap(kCheckIconPath));
  ui_->InitSpellStatus->setMovie(gif_);
  ui_->InitEnchantStatus->setMovie(gif_);
  FillProgressBarToMax();

  ui_->SpellTableWidget->blockSignals(true);
  ui_->SpellTableWidget->setUpdatesEnabled(false);

  ui_->EnchantTableWidget->blockSignals(true);
  ui_->EnchantTableWidget->setUpdatesEnabled(false);

  spell_table_thread_->start();
  enchant_table_thread_->start();

  spell_table_thread_->quit();
  enchant_table_thread_->quit();
}

void SirusPatcherWindow::OnSpellTableCreated() {
  for (int i = 0; i < ui_->SpellTableWidget->rowCount(); ++i) {
    ui_->SpellTableWidget->setCellWidget(i, 0, CreateCheckBox(true));
  }

  spell_json_table_->SetJsonTable(kJsonSpellTablePath);

  ui_->SpellTableWidget->blockSignals(false);
  ui_->SpellTableWidget->setUpdatesEnabled(true);

  ui_->SpellTableWidget->setSortingEnabled(true);
  ui_->SpellTableWidget->sortItems(2, Qt::AscendingOrder);
  ui_->SpellTableWidget->update();

  ui_->SpellTableWidget->viewport()->update();

  ui_->InitSpellStatus->setPixmap(QPixmap(kCheckIconPath));
  ui_->MainTabWidget->setTabEnabled(kSpellTab, true);

  TryEnableButtons();
}

void SirusPatcherWindow::OnEnchantTableCreated() {
  for (int i = 0; i < ui_->EnchantTableWidget->rowCount(); ++i) {
    ui_->EnchantTableWidget->setCellWidget(i, 0, CreateCheckBox(true));
  }

  enchant_json_table_->SetJsonTable(kJsonEnchantTablePath);

  ui_->EnchantTableWidget->blockSignals(false);
  ui_->EnchantTableWidget->setUpdatesEnabled(true);

  ui_->EnchantTableWidget->setSortingEnabled(true);
  ui_->EnchantTableWidget->sortItems(2, Qt::AscendingOrder);
  ui_->EnchantTableWidget->update();

  ui_->EnchantTableWidget->viewport()->update();

  ui_->InitEnchantStatus->setPixmap(QPixmap(kCheckIconPath));
  ui_->MainTabWidget->setTabEnabled(kEnchantTab, true);

  TryEnableButtons();
}

void SirusPatcherWindow::OnErrorOccurred(const QString& error) {
  is_error_occurred_ = true;
  QMessageBox::critical(this, "Ошибка!", error);
}

void SirusPatcherWindow::OnWarningOccurred(const QString& warning) {
  QMessageBox::warning(this, "Предупреждение!", warning);
}

void SirusPatcherWindow::OnPatchCreated() {
  EnableButtons();
  EnableTabs();
  FillProgressBarToMax();
  if (!is_error_occurred_) {
    QMessageBox::information(this, "Патч создан",
                             "Патч успешно создан, приятной игры!");
  }
  is_error_occurred_ = false;
}

#pragma region TableMethods

void SirusPatcherWindow::SetRowCountTable(QTableWidget* table, int count) {
  table->setRowCount(count);
}

void SirusPatcherWindow::ResetRowCountTable(QTableWidget* table, int count) {
  table->setRowCount(0);
  table->setSortingEnabled(false);
  table->setRowCount(count);
}

void SirusPatcherWindow::AddItemTable(QTableWidget* table, int row, int column,
                                      const QVariant& text) {
  QTableWidgetItem* item = new QTableWidgetItem();
  item->setData(Qt::DisplayRole, text);
  table->setItem(row, column, item);
}

#pragma endregion TableMethods

#pragma region ProgressBar

void SirusPatcherWindow::AddProgressBarValue(int value) {
  ui_->ProgressBar->setValue(
      qMin(ui_->ProgressBar->value() + value, ui_->ProgressBar->maximum()));
}

void SirusPatcherWindow::SetProgressBarMaximum(int maximum) {
  ui_->ProgressBar->setMaximum(maximum);
  ui_->ProgressBar->setValue(0);
  total_records_count_ = 0;
}

void SirusPatcherWindow::ProgressBarClear() {
  ui_->ProgressBar->setValue(0);
  total_records_count_ = 0;
}

void SirusPatcherWindow::FillProgressBarToMax() {
  ui_->ProgressBar->setValue(ui_->ProgressBar->maximum());
  total_records_count_ = 0;
}

void SirusPatcherWindow::AddTotalRecords(int count) {
  total_records_count_ += count;
  ui_->ProgressBar->setMaximum(total_records_count_);
}

#pragma endregion ProgressBar

#pragma endregion Slots

#pragma region InitMethods

void SirusPatcherWindow::InitThreadsAndWorkers() {
  mpq_archiver_ = new MPQArchiver();
  spell_table_worker_ = new SpellTableWorker(ui_->SpellTableWidget);
  enchant_table_worker_ = new EnchantTableWorker(ui_->EnchantTableWidget);
  create_patch_worker_ = new CreatePatchWorker(
      mpq_archiver_, spell_table_worker_, enchant_table_worker_);

  mpq_archiver_thread_ = new QThread();
  spell_table_thread_ = new QThread();
  enchant_table_thread_ = new QThread();
  create_patch_thread_ = new QThread();

  mpq_archiver_->moveToThread(mpq_archiver_thread_);
  spell_table_worker_->moveToThread(spell_table_thread_);
  enchant_table_worker_->moveToThread(enchant_table_thread_);
  create_patch_worker_->moveToThread(create_patch_thread_);
}

void SirusPatcherWindow::InitTabs() {
  struct TabInfo {
    int index;
    QString title;
    bool initiallyEnabled;
  };

  TabInfo tabs[] = {{kSettingsTab, "Настройки", true},
                    {kSpellTab, "Заклинания", false},
                    {kEnchantTab, "Иллюзии", false},
                    {kFeaturesTab, "Доп. возможности", false},
                    {kAboutTab, "О программе", true}};

  QFont tabFont("Segoe UI");
  tabFont.setPointSize(12);

  for (const auto& tab : tabs) {
    QLabel* label = new QLabel(tab.title);
    label->setFont(tabFont);
    label->setAlignment(Qt::AlignCenter);
    label->setGeometry(0, 0, 150, 50);

    ui_->MainTabWidget->tabBar()->setTabButton(tab.index, QTabBar::LeftSide,
                                               label);

    ui_->MainTabWidget->setTabEnabled(tab.index, tab.initiallyEnabled);
  }
}

void SirusPatcherWindow::InitSpellTable() {
  QStringList headers;
  headers << "*"
          << "ID"
          << "Название"
          << "Описание";
  ui_->SpellTableWidget->setColumnCount(headers.size());
  ui_->SpellTableWidget->setHorizontalHeaderLabels(headers);

  ui_->SpellTableWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Fixed);
  ui_->SpellTableWidget->verticalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);

  ui_->SpellTableWidget->setColumnWidth(0, 35);
  ui_->SpellTableWidget->setColumnWidth(1, 60);
  ui_->SpellTableWidget->setColumnWidth(2, 200);
  ui_->SpellTableWidget->setColumnWidth(3, 510);
  ui_->SpellTableWidget->setUpdatesEnabled(true);
}

void SirusPatcherWindow::InitEnchantTable() {
  QStringList headers;
  headers << "*"
          << "ID"
          << "Название";
  ui_->EnchantTableWidget->setColumnCount(headers.size());
  ui_->EnchantTableWidget->setHorizontalHeaderLabels(headers);

  ui_->EnchantTableWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Fixed);
  ui_->EnchantTableWidget->verticalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);

  ui_->EnchantTableWidget->setColumnWidth(0, 35);
  ui_->EnchantTableWidget->setColumnWidth(1, 60);
  ui_->EnchantTableWidget->setColumnWidth(2, 710);
  ui_->EnchantTableWidget->setUpdatesEnabled(true);
}

void SirusPatcherWindow::InitEventHandlers() {
  ConnectButtons();
  ConnectMPQArchiver();
  ConnectSpellTable();
  ConnectEnchantTable();
  ConnectCreatePatch();
}

#pragma endregion InitMethods

#pragma region ConnectSignals

void SirusPatcherWindow::ConnectButtons() {
  connect(ui_->ChooseDirectoryButton, &QPushButton::clicked, this,
          &SirusPatcherWindow::OnChooseDirectoryButtonClicked);
  connect(ui_->CreatePatchButton, &QPushButton::clicked, this,
          &SirusPatcherWindow::OnCreatePatchButtonClicked);

  connect(ui_->EnableAllCBButton, &QPushButton::clicked, this,
          &SirusPatcherWindow::OnEnableAllCBButtonClicked);
  connect(ui_->DisableAllCBButton, &QPushButton::clicked, this,
          &SirusPatcherWindow::OnDisableAllCBButtonClicked);

  connect(ui_->MainTabWidget, &QTabWidget::tabBarClicked, this,
          &SirusPatcherWindow::OnTabBarClicked);

  connect(ui_->SpellTableWidget, &QAbstractItemView::clicked, this,
          &SirusPatcherWindow::OnItemClicked);
  connect(ui_->EnchantTableWidget, &QAbstractItemView::clicked, this,
          &SirusPatcherWindow::OnItemClicked);
}

void SirusPatcherWindow::ConnectMPQArchiver() {
  connect(mpq_archiver_thread_, &QThread::started, mpq_archiver_,
          qOverload<>(&MPQArchiver::ExtractDBCFiles));
  connect(mpq_archiver_thread_, &QThread::finished, this,
          &SirusPatcherWindow::OnExtractingFinished);
  connect(mpq_archiver_, &MPQArchiver::ErrorOccurred, this,
          &SirusPatcherWindow::OnErrorOccurred);
}

void SirusPatcherWindow::ConnectSpellTable() {
  connect(spell_table_thread_, &QThread::started, spell_table_worker_,
          &SpellTableWorker::InitDBCTable);
  connect(spell_table_thread_, &QThread::finished, this,
          &SirusPatcherWindow::OnSpellTableCreated);

  connect(spell_table_worker_, &SpellTableWorker::SetRowCount, this,
          &SirusPatcherWindow::SetRowCountTable);
  connect(spell_table_worker_, &SpellTableWorker::ResetRowCount, this,
          &SirusPatcherWindow::ResetRowCountTable);
  connect(spell_table_worker_, &SpellTableWorker::AddItem, this,
          &SirusPatcherWindow::AddItemTable);
  connect(spell_table_worker_, &SpellTableWorker::AddProgressBarValue, this,
          &SirusPatcherWindow::AddProgressBarValue);
  connect(spell_table_worker_, &SpellTableWorker::SetProgressBarMaximum, this,
          &SirusPatcherWindow::SetProgressBarMaximum);
  connect(spell_table_worker_, &SpellTableWorker::AddTotalRecords, this,
          &SirusPatcherWindow::AddTotalRecords);
  connect(spell_table_worker_, &SpellTableWorker::ErrorOccurred, this,
          &SirusPatcherWindow::OnErrorOccurred);
  connect(spell_table_worker_, &SpellTableWorker::WarningOccurred, this,
          &SirusPatcherWindow::OnWarningOccurred);
}

void SirusPatcherWindow::ConnectEnchantTable() {
  connect(enchant_table_thread_, &QThread::started, enchant_table_worker_,
          &EnchantTableWorker::InitDBCTable);
  connect(enchant_table_thread_, &QThread::finished, this,
          &SirusPatcherWindow::OnEnchantTableCreated);

  connect(enchant_table_worker_, &EnchantTableWorker::ResetRowCount, this,
          &SirusPatcherWindow::ResetRowCountTable);
  connect(enchant_table_worker_, &EnchantTableWorker::AddItem, this,
          &SirusPatcherWindow::AddItemTable);
  connect(enchant_table_worker_, &EnchantTableWorker::AddProgressBarValue, this,
          &SirusPatcherWindow::AddProgressBarValue);
  connect(enchant_table_worker_, &EnchantTableWorker::SetProgressBarMaximum,
          this, &SirusPatcherWindow::SetProgressBarMaximum);
  connect(enchant_table_worker_, &EnchantTableWorker::AddTotalRecords, this,
          &SirusPatcherWindow::AddTotalRecords);
  connect(enchant_table_worker_, &EnchantTableWorker::ErrorOccurred, this,
          &SirusPatcherWindow::OnErrorOccurred);
  connect(enchant_table_worker_, &EnchantTableWorker::WarningOccurred, this,
          &SirusPatcherWindow::OnWarningOccurred);
}

void SirusPatcherWindow::ConnectCreatePatch() {
  connect(create_patch_thread_, &QThread::started, create_patch_worker_,
          &CreatePatchWorker::StartCreatingPatch);
  connect(create_patch_thread_, &QThread::finished, this,
          &SirusPatcherWindow::OnPatchCreated);
  connect(create_patch_worker_, &CreatePatchWorker::AddProgressBarValue, this,
          &SirusPatcherWindow::AddProgressBarValue);
  connect(create_patch_worker_, &CreatePatchWorker::SetProgressBarMaximum, this,
          &SirusPatcherWindow::SetProgressBarMaximum);
  connect(create_patch_worker_, &CreatePatchWorker::AddTotalRecords, this,
          &SirusPatcherWindow::AddTotalRecords);
  connect(create_patch_worker_, &CreatePatchWorker::ErrorOccurred, this,
          &SirusPatcherWindow::OnErrorOccurred);
}

#pragma endregion ConnectSignals

#pragma region HelperMethods

bool SirusPatcherWindow::ValidateGameDirectory(QString& dir) {
  if (dir.isEmpty()) {
    return false;
  }

  QString run_exe = dir + "/run.exe";
  if (!QFile::exists(run_exe)) {
    QMessageBox::critical(this, "Ошибка выбора директории",
                          "Файл запуска игры не обнаружен!\n"
                          "Проверьте следующий путь:\n" +
                              run_exe);
    return false;
  }

  QString mpq_dir = dir + '/' + MPQArchiver::kImportPatchDirectory;
  if (!QFile::exists(mpq_dir)) {
    QMessageBox::critical(this, "Ошибка выбора директории",
                          "MPQ архив не обнаружен!\n"
                          "Проверьте следующий путь:\n" +
                              mpq_dir);
    return false;
  }

  return true;
}

void SirusPatcherWindow::ReloadProgramState() {
  ui_->ChooseDirectoryButton->setEnabled(false);
  ui_->CreatePatchButton->setEnabled(false);
  ui_->MainTabWidget->setTabEnabled(kSpellTab, false);
  ui_->MpqUnpackedStatus->setPixmap(QPixmap(kMinusIconPath));
  ui_->InitSpellStatus->setPixmap(QPixmap(kMinusIconPath));
  ui_->InitEnchantStatus->setPixmap(QPixmap(kMinusIconPath));
}

void SirusPatcherWindow::SetTableCheckBoxes(QTableWidget* table, bool state) {
  for (int i = 0; i < table->rowCount(); ++i) {
    QWidget* item = table->cellWidget(i, 0);
    QCheckBox* check_box =
        qobject_cast<QCheckBox*>(item->layout()->itemAt(0)->widget());
    check_box->setChecked(state);
  }
}

QWidget* SirusPatcherWindow::CreateCheckBox(bool state) {
  QWidget* widget = new QWidget();
  QCheckBox* check_box = new QCheckBox();
  QHBoxLayout* layout = new QHBoxLayout(widget);
  layout->addWidget(check_box);
  layout->setAlignment(Qt::AlignCenter);
  layout->setContentsMargins(0, 0, 0, 0);
  check_box->setChecked(state);
  return widget;
}

void SirusPatcherWindow::TryEnableButtons() {
  if (++count_prepared_tables_ >= 2) {
    ui_->CreatePatchButton->setEnabled(true);
    ui_->ChooseDirectoryButton->setEnabled(true);
    delete gif_, gif_ = nullptr;
    count_prepared_tables_ = 0;
  }
}

void SirusPatcherWindow::EnableButtons() {
  ui_->ChooseDirectoryButton->setEnabled(true);
  ui_->EnableAllCBButton->setEnabled(true);
  ui_->DisableAllCBButton->setEnabled(true);
  ui_->CreatePatchButton->setEnabled(true);
}

void SirusPatcherWindow::DisableButtons() {
  ui_->ChooseDirectoryButton->setEnabled(false);
  ui_->EnableAllCBButton->setEnabled(false);
  ui_->DisableAllCBButton->setEnabled(false);
  ui_->CreatePatchButton->setEnabled(false);
}

void SirusPatcherWindow::EnableTabs() {
  ui_->SettingsTab->setEnabled(true);
  ui_->SpellTab->setEnabled(true);
  ui_->EnchantTab->setEnabled(true);
}

void SirusPatcherWindow::DisableTabs() {
  ui_->SettingsTab->setEnabled(false);
  ui_->SpellTab->setEnabled(false);
  ui_->EnchantTab->setEnabled(false);
}

#pragma endregion HelperMethods
