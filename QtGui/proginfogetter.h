#ifndef _PROGINFOGETTER_H_
#define _PROGINFOGETTER_H_

#include <QString>

class ProgInfoGetter {
 public:
  static const QString& GetProgName();
  static const QString& GetVersion();

 private:
  ProgInfoGetter();
  static const QString progname_;
  static QString version_;
};

#endif  // _PROGINFOGETTER_H_
