#include <QApplication>
#include <QPixmap>

#include "QtGui/siruspatcherwindow.h"
#include "QtGui/splashloadingscreen.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QPixmap pixmap("://resources/images/splash-screen.png");
  SplashLoadingScreen loading_screen(
      pixmap, Qt::SplashScreen | Qt::WindowStaysOnTopHint);
  loading_screen.Start(&app);

  SirusPatcherWindow window;
  window.show();
  loading_screen.finish(&window);

  return app.exec();
}
