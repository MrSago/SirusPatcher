#include "QtGui/siruspatcherwindow.h"

#include <QCheckBox>
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
#include "QtGui/proginfogetter.h"

SirusPatcherWindow::SirusPatcherWindow(QWidget* parent)
    : QMainWindow(parent), ui_(new Ui::SirusPatcherWindow) {
  ui_->setupUi(this);

  SetupWindow();
  SetupConnections();
  SetupTabLabels();
}

SirusPatcherWindow::~SirusPatcherWindow() {
  delete spell_table_;
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
  if (game_dir.isEmpty()) return;

  if (!QFile::exists(game_dir + "/run.exe")) {
    QMessageBox::critical(this, "Ошибка выбора директории",
                          "Файл запуска игры run.exe не обнаружен!");
    return;
  }

  ui_->DirectoryLine->setText(game_dir);

  // TODO:
  // Extract DBC files from MPQ...

  // ...

  spell_table_ = new SpellDBCTable(ui_->SpellTableWidget);
  spell_table_->SetupTable("./Spell.dbc", "://dbc/Spell.dbc.json");

  for (int i = 1; i < ui_->MainTabWidget->count() - 1; ++i) {
    ui_->MainTabWidget->setTabEnabled(i, true);
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
