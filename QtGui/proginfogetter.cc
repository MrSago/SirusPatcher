#include "QtGui/proginfogetter.h"

#include <QFile>
#include <QString>

const QString ProgInfoGetter::progname_ = "Sirus Patcher";
QString ProgInfoGetter::version_ = "";

ProgInfoGetter::ProgInfoGetter() {}

const QString& ProgInfoGetter::GetProgName() { return progname_; }

const QString& ProgInfoGetter::GetVersion() {
  if (ProgInfoGetter::version_.size()) {
    return ProgInfoGetter::version_;
  }

  QFile file("://text/version");
  file.open(QIODevice::ReadOnly);

  ProgInfoGetter::version_ = file.readAll();
  return ProgInfoGetter::version_;
}
