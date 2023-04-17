#include "QtGui/siruspatcherwindow.h"

#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QTabBar>
#include <QWindow>

#include "./ui_siruspatcherwindow.h"
#include "QtGui/proginfogetter.h"

SirusPatcherWindow::SirusPatcherWindow(QWidget* parent)
    : QMainWindow(parent), ui_(new Ui::SirusPatcherWindow) {
  ui_->setupUi(this);

  this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
  ui_->WindowName->setText(QString("%1 %2").arg(ProgInfoGetter::GetProgName(),
                                                ProgInfoGetter::GetVersion()));

  connect(ui_->HideButton, &QPushButton::clicked, this,
          &QMainWindow::showMinimized);
  connect(ui_->CloseButton, &QPushButton::clicked, this, &QMainWindow::close);

  //  ui_->tabWidget->tabBar()->setTabButton(0, QTabBar::LeftSide,
  //                                         new QLabel("kekw"));
  //  ui_->tabWidget->tabBar()->setTabButton(1, QTabBar::LeftSide,
  //                                         new QLabel("chekis"));
  //  ui_->tabWidget->setStyleSheet(
  //      "QTabBar::tab { height: 100px; width: 100px; }");
}

SirusPatcherWindow::~SirusPatcherWindow() { delete ui_; }
