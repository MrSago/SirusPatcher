#include "QtGui/siruspatcherwindow.h"

#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QMovie>
#include <QPixmap>
#include <QPushButton>
#include <QTabBar>
#include <QTableWidget>
#include <QWindow>

#include "./ui_siruspatcherwindow.h"
#include "QtGui/mpqworker.h"
#include "QtGui/proginfogetter.h"

SirusPatcherWindow::SirusPatcherWindow(QWidget* parent)
    : QMainWindow(parent), ui_(new Ui::SirusPatcherWindow) {
  ui_->setupUi(this);

  SetupWindow();
  SetupConnections();
  SetupTabLabels();

  spell_table_ = new SpellDBCTable(ui_->SpellTableWidget);
}

SirusPatcherWindow::~SirusPatcherWindow() {
  delete spell_table_;
  delete ui_;
}

void SirusPatcherWindow::OnChooseDirectoryButtonClicked() {
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
    if (started_dir != QDir::currentPath()) {
      ui_->DirectoryCorrectStatus->setPixmap(QPixmap(kCheckIconPath));
    } else {
      ui_->DirectoryCorrectStatus->setPixmap(QPixmap(kCrossIconPath));
    }
    delete gif_;
    return;
  }
  ui_->DirectoryCorrectStatus->setPixmap(QPixmap(kCheckIconPath));

  ReloadProgramState();
  ui_->GameDirectoryLine->setText(game_dir);
  ui_->ProgressBar->setValue(30);

  if (!SetupTables()) {
    delete gif_;
    return;
  }

  ui_->CreatePatchButton->setEnabled(true);
  ui_->ProgressBar->setValue(100);
  delete gif_;
}

void SirusPatcherWindow::OnCreatePatchButtonClicked() {
  bool success = spell_table_->WriteValuesToFile();
  if (!success) {
    QMessageBox::critical(
        this, "Ошибка записи в DBC файл",
        "Ошибка при записи значений таблицы в файл: " + kDbcFileList[0]);
    return;
  }
  ui_->ProgressBar->setValue(50);

  success =
      MPQWorker::ArchiveDBCFiles(kDbcFileList, ui_->GameDirectoryLine->text());
  if (!success) {
    QMessageBox::critical(this, "Ошибка создания MPQ архива",
                          "Ошибка при создании MPQ архива");
    return;
  }

  ui_->ProgressBar->setValue(100);
  QMessageBox::information(this, "Патч создан",
                           "Патч успешно создан, приятной игры!");
}

void SirusPatcherWindow::OnEnableAllCBSPellButtonClicked() {
  spell_table_->SetTableCheckBoxes(true);
}

void SirusPatcherWindow::OnDisableAllCBSpellButtonClicked() {
  spell_table_->SetTableCheckBoxes(false);
}

void SirusPatcherWindow::ProgressBarClear() {
  if (ui_->ProgressBar->value() >= ui_->ProgressBar->maximum()) {
    ui_->ProgressBar->setValue(0);
  }
}

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

  QString run_exe = dir + "/run.exe";
  if (!QFile::exists(run_exe)) {
    QMessageBox::critical(this, "Ошибка выбора директории",
                          "Файл запуска игры не обнаружен!\n"
                          "Проверьте следующий путь:\n" +
                              run_exe);
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
  ui_->MpqUnpackedStatus->setMovie(gif_);

  bool success = MPQWorker::ExtractDBCFiles(
      kDbcFileList, ui_->GameDirectoryLine->text(), QDir::currentPath());
  if (!success) {
    QMessageBox::critical(
        this, "Ошибка импорта DBC файлов",
        "Ошибка при импорте DBC файлов:\n" + kDbcFileList.join('\n'));
    ui_->MpqUnpackedStatus->setPixmap(QPixmap(kCrossIconPath));
    return false;
  }

  ui_->ProgressBar->setValue(ui_->ProgressBar->value() + 30);
  ui_->MpqUnpackedStatus->setPixmap(QPixmap(kCheckIconPath));
  ui_->InitSpellStatus->setMovie(gif_);

  success = spell_table_->SetupFromFile();
  if (!success) {
    QMessageBox::critical(this, "Ошибка инициализации таблицы",
                          "Ошибка при инициализации таблицы: spell_table_");
    ui_->InitSpellStatus->setPixmap(QPixmap(kCrossIconPath));
    return false;
  }

  ui_->MainTabWidget->setTabEnabled(1, true);
  ui_->ProgressBar->setValue(ui_->ProgressBar->value() + 30);
  ui_->InitSpellStatus->setPixmap(QPixmap(kCheckIconPath));

  return true;
}

void SirusPatcherWindow::ReloadProgramState() {
  ui_->CreatePatchButton->setEnabled(false);
  ui_->SpellTab->setEnabled(false);
  ui_->MpqUnpackedStatus->setPixmap(QPixmap(kMinusIconPath));
  ui_->InitSpellStatus->setPixmap(QPixmap(kMinusIconPath));
}
