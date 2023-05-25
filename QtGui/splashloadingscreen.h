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
 private:
  static const QString kDisclaimerText;
  static const QString kLoadingLabelText;
  static const QString kMainFont;
  static const QString kIconFile;

  static constexpr int kLoadTimeMS = 3 * 1000;

  static constexpr int kProgressPixelsX = 394;
  static constexpr int kProgressPixelsY = 369;
  static constexpr int kProgressPixelsWidth = 237;
  static constexpr int kProgressPixelsHeight = 35;

  static constexpr QPoint kProgramNamePoint = {410, 50};
  static constexpr int kProgramNameSize = 20;

  static constexpr QPoint kIconPoint = {478, 80};

  static constexpr int kDisclaimerPixelsX = 390;
  static constexpr int kDisclaimerPixelsY = 90;
  static constexpr int kDisclaimerPixelsWidth = 240;
  static constexpr int kDisclaimerPixelsHeight = 260;
  static constexpr int kDisclaimerSize = 10;

  static constexpr QPoint kLoadingLabelPoint = {390, 355};
  static constexpr int kLoadingLabelSize = 15;

 public:
  explicit SplashLoadingScreen(const QPixmap& pixmap = QPixmap(),
                               Qt::WindowFlags flags = Qt::WindowFlags());
  void Start(QApplication* app);

 private:
  bool eventFilter(QObject* target, QEvent* event) override;
  void PreparePixmap();
  void SetHeader(QPainter& painter, QFont& font);
  void SetIcon(QPainter& painter);
  void SetDisclaimer(QPainter& painter, QFont& font);
  void SetLoadingLabel(QPainter& painter, QFont& font);

  QPixmap pixmap_;
};

#endif  // _SPLASHLOADINGSCREEN_H_
