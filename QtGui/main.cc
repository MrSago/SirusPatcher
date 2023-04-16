#include <QApplication>

#include "QtGui/siruspatcherwindow.h"

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  SirusPatcherWindow w;
  w.show();
  return a.exec();
}
