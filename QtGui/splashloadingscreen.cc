#include "QtGui/splashloadingscreen.h"

#include <QApplication>
#include <QElapsedTimer>
#include <QEvent>
#include <QFont>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QRectF>
#include <QTextOption>
#include <QWidget>

#include "QtGui/proginfogetter.h"

SplashLoadingScreen::SplashLoadingScreen(const QPixmap& pixmap,
                                         Qt::WindowFlags flags)
    : QSplashScreen(pixmap, flags), pixmap_(pixmap) {
  PreparePixmap();
  this->installEventFilter(this);
}

void SplashLoadingScreen::Start(QApplication* app) {
  this->show();
  app->processEvents();

  QElapsedTimer timer;
  timer.start();
  while (timer.elapsed() < kLoadTimeMS) {
    const int progress =
        static_cast<double>(timer.elapsed()) / kLoadTimeMS * 100.0;
    this->showMessage(QString("Загружено: %1%").arg(progress),
                      Qt::AlignBottom | Qt::AlignRight);

    QPainter painter;
    painter.begin(&pixmap_);
    painter.fillRect(kProgressPixelsX, kProgressPixelsY,
                     progress / 100.0 * kProgressPixelsWidth,
                     kProgressPixelsHeight, Qt::darkBlue);
    painter.end();

    this->setPixmap(pixmap_);
    app->processEvents();
  }
}

bool SplashLoadingScreen::eventFilter(QObject* target, QEvent* event) {
  Q_UNUSED(target)
  if (event->type() == QEvent::MouseButtonPress ||
      event->type() == QEvent::MouseButtonDblClick ||
      event->type() == QEvent::MouseButtonRelease ||
      event->type() == QEvent::KeyPress ||
      event->type() == QEvent::KeyRelease) {
    return true;
  }
  return false;
}

void SplashLoadingScreen::PreparePixmap() {
  QPainter painter(&pixmap_);
  QFont font(kMainFont);

  SetHeader(painter, font);
  SetIcon(painter);
  SetDisclaimer(painter, font);
  SetLoadingLabel(painter, font);
}

void SplashLoadingScreen::SetHeader(QPainter& painter, QFont& font) {
  font.setPointSize(kProgramNameSize);
  painter.setFont(font);
  painter.drawText(kProgramNamePoint,
                   QString("%1 %2").arg(ProgInfoGetter::GetProgName(),
                                        ProgInfoGetter::GetVersion()));
}

void SplashLoadingScreen::SetIcon(QPainter& painter) {
  QImage icon(kIconFile);
  painter.drawImage(kIconPoint, icon);
}

void SplashLoadingScreen::SetDisclaimer(QPainter& painter, QFont& font) {
  QTextOption text_option;
  text_option.setWrapMode(QTextOption::WrapMode::WordWrap);
  text_option.setAlignment(Qt::AlignCenter);
  font.setPointSize(kDisclaimerSize);
  painter.setFont(font);
  painter.drawText(QRectF(kDisclaimerPixelsX, kDisclaimerPixelsY,
                          kDisclaimerPixelsWidth, kDisclaimerPixelsHeight),
                   kDisclaimerText, text_option);
}

void SplashLoadingScreen::SetLoadingLabel(QPainter& painter, QFont& font) {
  font.setPointSize(kLoadingLabelSize);
  painter.setFont(font);
  painter.drawText(kLoadingLabelPoint, kLoadingLabelText);
}
