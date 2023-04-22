#include "QtGui/siruspatcherwindow.h"

#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
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
#include "QtGui/mpqarchiver.h"
#include "QtGui/proginfogetter.h"
#include "QtGui/spelltableworker.h"

SirusPatcherWindow::SirusPatcherWindow(QWidget* parent)
    : QMainWindow(parent), ui_(new Ui::SirusPatcherWindow), gif_(nullptr) {
  ui_->setupUi(this);
  InitThreadsAndWorkers();
  SetupWindow();
  SetupTabLabels();
  SetupSpellTable();
  SetupConnections();
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

  create_patch_thread_->quit();
  create_patch_thread_->wait();
  delete create_patch_thread_;
  delete create_patch_worker_;

  if (gif_) delete gif_;

  delete ui_;
}

void SirusPatcherWindow::OnChooseDirectoryButtonClicked() {
  ProgressBarClear();
  gif_ = new QMovie(kUpdatedGifPath);
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
    delete gif_, gif_ = nullptr;
    return;
  }
  ui_->DirectoryCorrectStatus->setPixmap(QPixmap(kCheckIconPath));
  ui_->GameDirectoryLine->setText(game_dir);

  ReloadProgramState();
  AddProgressBarValue(30);

  ui_->MpqUnpackedStatus->setMovie(gif_);
  mpq_archiver_->SetParameters(kDbcFileList, game_dir, QDir::currentPath());
  mpq_archiver_thread_->start();
  mpq_archiver_thread_->quit();
}

void SirusPatcherWindow::OnCreatePatchButtonClicked() {
  ProgressBarClear();
  ui_->CreatePatchButton->setEnabled(false);
  ui_->SettingsTab->setEnabled(false);
  ui_->SpellTab->setEnabled(false);

  create_patch_thread_->start();
  create_patch_thread_->quit();
}

void SirusPatcherWindow::OnEnableAllCBSpellButtonClicked() {
  SetTableCheckBoxes(ui_->SpellTableWidget, true);
}

void SirusPatcherWindow::OnDisableAllCBSpellButtonClicked() {
  SetTableCheckBoxes(ui_->SpellTableWidget, false);
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

void SirusPatcherWindow::OnExtractingFinished() {
  ui_->MpqUnpackedStatus->setPixmap(QPixmap(kCheckIconPath));
  ui_->InitSpellStatus->setMovie(gif_);
  AddProgressBarValue(100);
  spell_table_thread_->start();
  spell_table_thread_->quit();
}

void SirusPatcherWindow::OnSpellTableCreated() {
  for (int i = 0; i < ui_->SpellTableWidget->rowCount(); ++i) {
    ui_->SpellTableWidget->setCellWidget(i, 0, CreateCheckBox(true));
  }
  ui_->SpellTableWidget->setSortingEnabled(true);
  ui_->SpellTableWidget->sortItems(2, Qt::AscendingOrder);

  delete gif_, gif_ = nullptr;
  ui_->InitSpellStatus->setPixmap(QPixmap(kCheckIconPath));
  ui_->MainTabWidget->setTabEnabled(kSpellTab, true);
  ui_->CreatePatchButton->setEnabled(true);

  ui_->ChooseDirectoryButton->setEnabled(true);
  ui_->SpellTableWidget->update();
}

void SirusPatcherWindow::OnErrorOccurred(const QString& error) {
  QMessageBox::critical(this, "Ошибка", error);
}

void SirusPatcherWindow::OnPatchCreated() {
  ui_->CreatePatchButton->setEnabled(true);
  ui_->SettingsTab->setEnabled(true);
  ui_->SpellTab->setEnabled(true);
  AddProgressBarValue(100);
  QMessageBox::information(this, "Патч создан",
                           "Патч успешно создан, приятной игры!");
}

void SirusPatcherWindow::AddProgressBarValue(int value) {
  ui_->ProgressBar->setValue(
      qMin(ui_->ProgressBar->value() + value, ui_->ProgressBar->maximum()));
}

void SirusPatcherWindow::ProgressBarClear() {
  if (ui_->ProgressBar->value() >= ui_->ProgressBar->maximum()) {
    ui_->ProgressBar->setValue(0);
  }
}

void SirusPatcherWindow::InitThreadsAndWorkers() {
  mpq_archiver_ = new MPQArchiver();
  spell_table_worker_ = new SpellTableWorker(ui_->SpellTableWidget);
  create_patch_worker_ =
      new CreatePatchWorker(mpq_archiver_, spell_table_worker_);

  mpq_archiver_thread_ = new QThread();
  spell_table_thread_ = new QThread();
  create_patch_thread_ = new QThread();

  mpq_archiver_->moveToThread(mpq_archiver_thread_);
  spell_table_worker_->moveToThread(spell_table_thread_);
  create_patch_worker_->moveToThread(create_patch_thread_);
}

void SirusPatcherWindow::SetupWindow() {
  this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
  ui_->WindowName->setText(QString("%1 %2").arg(ProgInfoGetter::GetProgName(),
                                                ProgInfoGetter::GetVersion()));
}

void SirusPatcherWindow::SetupTabLabels() {
  ui_->MainTabWidget->tabBar()->setTabButton(kSettingsTab, QTabBar::LeftSide,
                                             TabLabel("Настройки"));
  ui_->MainTabWidget->tabBar()->setTabButton(kSpellTab, QTabBar::LeftSide,
                                             TabLabel("Заклинания"));
  ui_->MainTabWidget->tabBar()->setTabButton(kAboutTab, QTabBar::LeftSide,
                                             TabLabel("О программе"));

  ui_->MainTabWidget->setTabEnabled(kSpellTab, false);
}

void SirusPatcherWindow::SetupSpellTable() {
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

void SirusPatcherWindow::SetupConnections() {
  ConnectButtons();
  ConnectMPQArchiver();
  ConnectSpellTable();
  ConnectCreatePatch();
}

void SirusPatcherWindow::ConnectButtons() {
  connect(ui_->HideButton, &QPushButton::clicked, this,
          &QMainWindow::showMinimized);
  connect(ui_->CloseButton, &QPushButton::clicked, this, &QMainWindow::close);

  connect(ui_->ChooseDirectoryButton, &QPushButton::clicked, this,
          &SirusPatcherWindow::OnChooseDirectoryButtonClicked);
  connect(ui_->CreatePatchButton, &QPushButton::clicked, this,
          &SirusPatcherWindow::OnCreatePatchButtonClicked);

  connect(ui_->EnableAllCBSPellButton, &QPushButton::clicked, this,
          &SirusPatcherWindow::OnEnableAllCBSpellButtonClicked);
  connect(ui_->DisableAllCBSpellButton, &QPushButton::clicked, this,
          &SirusPatcherWindow::OnDisableAllCBSpellButtonClicked);

  connect(ui_->MainTabWidget, &QTabWidget::tabBarClicked, this,
          &SirusPatcherWindow::ProgressBarClear);
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

  connect(spell_table_worker_, &SpellTableWorker::ResetRowCount, this,
          &SirusPatcherWindow::ResetRowCountTable);
  connect(spell_table_worker_, &SpellTableWorker::AddItem, this,
          &SirusPatcherWindow::AddItemTable);
  connect(spell_table_worker_, &SpellTableWorker::ProgressChanged, this,
          &SirusPatcherWindow::AddProgressBarValue);
  connect(spell_table_worker_, &SpellTableWorker::ErrorOccurred, this,
          &SirusPatcherWindow::OnErrorOccurred);
}

void SirusPatcherWindow::ConnectCreatePatch() {
  connect(create_patch_thread_, &QThread::started, create_patch_worker_,
          &CreatePatchWorker::StartCreatingPatch);
  connect(create_patch_thread_, &QThread::finished, this,
          &SirusPatcherWindow::OnPatchCreated);
  connect(create_patch_worker_, &CreatePatchWorker::ProgressChanged, this,
          &SirusPatcherWindow::AddProgressBarValue);
}

QLabel* SirusPatcherWindow::TabLabel(const QString& text) {
  QLabel* label = new QLabel(text);
  QFont font("Segoe UI");
  font.setPointSize(12);
  label->setFont(font);
  label->setAlignment(Qt::AlignCenter);
  label->setGeometry(0, 0, 150, 50);
  return label;
}

bool SirusPatcherWindow::ValidateGameDirectory(QString& dir) {
  if (dir.isEmpty()) return false;

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
