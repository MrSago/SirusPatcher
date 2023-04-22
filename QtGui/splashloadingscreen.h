#ifndef _SPLASHLOADINGSCREEN_H_
#define _SPLASHLOADINGSCREEN_H_

#include <QApplication>
#include <QEvent>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QSplashScreen>
#include <QString>

class SplashLoadingScreen : public QSplashScreen {
 public:
  explicit SplashLoadingScreen(const QPixmap& pixmap = QPixmap(),
                      Qt::WindowFlags flags = Qt::WindowFlags());
  void Start(QApplication* app);

 private:
  const int kLoadTimeMS = 5 * 1000;

  const int kProgressPixelsX = 394;
  const int kProgressPixelsY = 369;
  const int kProgressPixelsWidth = 237;
  const int kProgressPixelsHeight = 35;

  const QString kMainFont = "Segoe UI";

  const QPoint kProgramNamePoint = {410, 50};
  const int kProgramNameSize = 20;

  const QString kIconFile = "://resources/icons/window.ico";
  const QPoint kIconPoint = {478, 80};

  const QString kDisclaimerText =
      "Разработчик не несёт ответственности за использование патча, созданный "
      "данной программой. "
      "Вы делаете всё на свой страх и риск! ©️ MrS4g0";
  const int kDisclaimerPixelsX = 390;
  const int kDisclaimerPixelsY = 90;
  const int kDisclaimerPixelsWidth = 240;
  const int kDisclaimerPixelsHeight = 260;
  const int kDisclaimerSize = 10;

  const QString kLoadingLabelText = "Загрузка, ожидайте...";
  const QPoint kLoadingLabelPoint = {390, 355};
  const int kLoadingLabelSize = 15;

  bool eventFilter(QObject* target, QEvent* event) override;
  void PreparePixmap();
  void SetHeader(QPainter& painter, QFont& font);
  void SetIcon(QPainter& painter);
  void SetDisclaimer(QPainter& painter, QFont& font);
  void SetLoadingLabel(QPainter& painter, QFont& font);

  QPixmap pixmap_;
};

#endif  // _SPLASHLOADINGSCREEN_H_
