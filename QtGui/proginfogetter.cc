#include "QtGui/proginfogetter.h"

#include <QFile>
#include <QIODevice>
#include <QString>

const QString ProgInfoGetter::progname_ = "Sirus Patcher";
QString ProgInfoGetter::version_ = "";

ProgInfoGetter::ProgInfoGetter() {}

const QString& ProgInfoGetter::GetProgName() { return progname_; }

const QString& ProgInfoGetter::GetVersion() {
  if (ProgInfoGetter::version_.length()) {
    return ProgInfoGetter::version_;
  }

  QFile file("://resources/text/version");
  file.open(QIODevice::ReadOnly);
  ProgInfoGetter::version_ = file.readAll();
  return ProgInfoGetter::version_;
}
