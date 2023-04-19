#include "QtGui/siruspatcherwindow.h"

#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QTabBar>
#include <QTableWidget>
#include <QWindow>

#include "./ui_siruspatcherwindow.h"
#include "QtGui/abstractdbctable.h"
#include "QtGui/mpqworker.h"
#include "QtGui/proginfogetter.h"

SirusPatcherWindow::SirusPatcherWindow(QWidget* parent)
    : QMainWindow(parent),
      ui_(new Ui::SirusPatcherWindow),
      spell_table_(nullptr) {
  ui_->setupUi(this);
  SetupWindow();
  SetupConnections();
  SetupTabLabels();
}

SirusPatcherWindow::~SirusPatcherWindow() {
  if (spell_table_) delete spell_table_;
  delete ui_;
}

void SirusPatcherWindow::OnChooseDirectoryButtonClicked() {
  QString started_path;
  if (QDir(ui_->GameDirectoryLine->text()).exists()) {
    started_path = ui_->GameDirectoryLine->text();
  } else {
    started_path = QDir::currentPath();
  }

  QString game_dir = QFileDialog::getExistingDirectory(
      this, "Выбор директории с игрой", started_path,
      QFileDialog::ShowDirsOnly);
  if (!ValidateGameDirectory(game_dir)) return;

  ReloadProgramState();
  ui_->GameDirectoryLine->setText(game_dir);
  ui_->ProgressBar->setValue(30);

  if (!SetupTables()) return;

  ui_->CreatePatchButton->setEnabled(true);

  ui_->ProgressBar->setValue(100);
}

void SirusPatcherWindow::OnCreatePatchButtonClicked() {
  if (!spell_table_) {
    return;
  }

  ui_->CreatePatchButton->setEnabled(false);

  bool success = spell_table_->WriteValuesToFile();
  if (!success) {
    QMessageBox::critical(
        this, "Ошибка записи в DBC файл",
        "Ошибка при записи значений таблицы в файл: Spell.dbc");
    return;
  }
  ui_->ProgressBar->setValue(50);

  QFile::remove(ui_->GameDirectoryLine->text() +
                MPQWorker::kExportPatchDirectory);
  success =
      MPQWorker::ArchiveDBCFiles(kDbcFileList, ui_->GameDirectoryLine->text());
  if (!success) {
    QMessageBox::critical(this, "Ошибка создания MPQ архива",
                          "Ошибка при создании MPQ архива");
    return;
  }

  ui_->ProgressBar->setValue(100);
}

void SirusPatcherWindow::OnEnableAllCBSPellButtonClicked() {
  SetTableCheckBoxes(ui_->SpellTableWidget, Qt::CheckState::Checked);
}

void SirusPatcherWindow::OnDisableAllCBSpellButtonClicked() {
  SetTableCheckBoxes(ui_->SpellTableWidget, Qt::CheckState::Unchecked);
}

void SirusPatcherWindow::ProgressBarClear() { ui_->ProgressBar->setValue(0); }

void SirusPatcherWindow::SetupWindow() {
  this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
  ui_->WindowName->setText(QString("%1 %2").arg(ProgInfoGetter::GetProgName(),
                                                ProgInfoGetter::GetVersion()));
}

void SirusPatcherWindow::SetupConnections() {
  connect(ui_->HideButton, &QPushButton::clicked, this,
          &QMainWindow::showMinimized);
  connect(ui_->CloseButton, &QPushButton::clicked, this, &QMainWindow::close);

  connect(ui_->ChooseDirectoryButton, &QPushButton::clicked, this,
          &SirusPatcherWindow::OnChooseDirectoryButtonClicked);
  connect(ui_->CreatePatchButton, &QPushButton::clicked, this,
          &SirusPatcherWindow::OnCreatePatchButtonClicked);
  connect(ui_->EnableAllCBSPellButton, &QPushButton::clicked, this,
          &SirusPatcherWindow::OnEnableAllCBSPellButtonClicked);
  connect(ui_->DisableAllCBSpellButton, &QPushButton::clicked, this,
          &SirusPatcherWindow::OnDisableAllCBSpellButtonClicked);

  connect(ui_->MainTabWidget, &QTabWidget::tabBarClicked, this,
          &SirusPatcherWindow::ProgressBarClear);
}

void SirusPatcherWindow::SetupTabLabels() {
  ui_->MainTabWidget->tabBar()->setTabButton(0, QTabBar::LeftSide,
                                             TabLabel("Настройки"));
  ui_->MainTabWidget->tabBar()->setTabButton(1, QTabBar::LeftSide,
                                             TabLabel("Заклинания"));
  ui_->MainTabWidget->tabBar()->setTabButton(2, QTabBar::LeftSide,
                                             TabLabel("О программе"));

  for (int i = 1; i < ui_->MainTabWidget->count() - 1; ++i) {
    ui_->MainTabWidget->setTabEnabled(i, false);
  }
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

  QString run_dir = dir + "/run.exe";
  if (!QFile::exists(run_dir)) {
    QMessageBox::critical(this, "Ошибка выбора директории",
                          "Файл запуска игры не обнаружен!\n"
                          "Проверьте следующий путь:\n" +
                              run_dir);
    return false;
  }

  QString mpq_dir = dir + '/' + MPQWorker::kImportPatchDirectory;
  if (!QFile::exists(mpq_dir)) {
    QMessageBox::critical(this, "Ошибка выбора директории",
                          "MPQ архив не обнаружен!\n"
                          "Проверьте следующий путь:\n" +
                              mpq_dir);
    return false;
  }

  return true;
}

bool SirusPatcherWindow::SetupTables() {
  bool success = MPQWorker::ExtractDBCFiles(
      kDbcFileList, ui_->GameDirectoryLine->text(), QDir::currentPath());
  if (!success) {
    QMessageBox::critical(
        this, "Ошибка импорта DBC файлов",
        "Ошибка при импорте DBC файлов:\n" + kDbcFileList.join('\n'));
    return false;
  }
  ui_->ProgressBar->setValue(ui_->ProgressBar->value() + 30);

  if (spell_table_) delete spell_table_;
  spell_table_ = new SpellDBCTable(ui_->SpellTableWidget, "./Spell.dbc",
                                   "://resources/dbc/Spell.dbc.json");
  success = spell_table_->SetupTableFromFile();
  if (!success) {
    QMessageBox::critical(this, "Ошибка инициализации таблицы",
                          "Ошибка при инициализации таблицы: SpellTable");
    return false;
  }
  ui_->MainTabWidget->setTabEnabled(1, true);
  ui_->ProgressBar->setValue(ui_->ProgressBar->value() + 30);

  return true;
}

void SirusPatcherWindow::ReloadProgramState() {
  ui_->CreatePatchButton->setEnabled(false);
  ui_->SpellTab->setEnabled(false);
  ui_->SpellTableWidget->clear();
  if (spell_table_) {
    delete spell_table_;
    spell_table_ = nullptr;
  }
}

void SirusPatcherWindow::SetTableCheckBoxes(QTableWidget* table,
                                            Qt::CheckState state) {
  for (int i = 0; i < table->rowCount(); ++i) {
    QWidget* item = table->cellWidget(i, 0);
    QCheckBox* check_box =
        qobject_cast<QCheckBox*>(item->layout()->itemAt(0)->widget());
    check_box->setCheckState(state);
  }
}
