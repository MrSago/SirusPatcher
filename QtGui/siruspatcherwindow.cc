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
  if (QDir(ui_->DirectoryLine->text()).exists()) {
    started_path = ui_->DirectoryLine->text();
  } else {
    started_path = QDir::currentPath();
  }

  QString game_dir = QFileDialog::getExistingDirectory(
      this, "Выбор директории с игрой", started_path,
      QFileDialog::ShowDirsOnly);
  if (!ValidateGameDirectory(game_dir)) return;
  ui_->DirectoryLine->setText(game_dir);

  if (!SetubTables()) return;

  // for (int i = 1; i < ui_->MainTabWidget->count() - 1; ++i) {
  //   ui_->MainTabWidget->setTabEnabled(i, true);
  // }
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
}

void SirusPatcherWindow::SetupTabLabels() {
  ui_->MainTabWidget->tabBar()->setTabButton(0, QTabBar::LeftSide,
                                             TabLabel("Настройки"));
  ui_->MainTabWidget->tabBar()->setTabButton(1, QTabBar::LeftSide,
                                             TabLabel("Заклинания"));
  ui_->MainTabWidget->tabBar()->setTabButton(2, QTabBar::LeftSide,
                                             TabLabel("О программе"));

  //  for (int i = 1; i < ui_->MainTabWidget->count() - 1; ++i) {
  //    ui_->MainTabWidget->setTabEnabled(i, false);
  //  }
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

bool SirusPatcherWindow::SetubTables() {
  const QStringList dbc_files = {"Spell.dbc" /*Another.dbc*/};

  bool success = MPQWorker::ExtractDBCFiles(
      dbc_files, ui_->DirectoryLine->text(), QDir::currentPath());
  if (!success) {
    QMessageBox::critical(
        this, "Ошибка импорта DBC файлов!",
        "Ошибка при импорте DBC файлов:\n" + dbc_files.join('\n'));
    return false;
  }

  spell_table_ = new SpellDBCTable(ui_->SpellTableWidget);
  success = spell_table_->SetupTable("./Spell.dbc",
                                     "://resources/dbc/Spell.dbc.json");
  if (!success) {
    QMessageBox::critical(this, "Ошибка инициализации таблицы",
                          "Ошибка при инициализации таблицы: SpellTable");
    return false;
  }

  return true;
}
