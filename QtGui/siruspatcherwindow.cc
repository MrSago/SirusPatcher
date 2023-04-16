#include "QtGui/siruspatcherwindow.h"

#include "./ui_siruspatcherwindow.h"

SirusPatcherWindow::SirusPatcherWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::SirusPatcherWindow) {
  ui->setupUi(this);
}

SirusPatcherWindow::~SirusPatcherWindow() { delete ui; }
