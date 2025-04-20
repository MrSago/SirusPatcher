#ifndef _JSONTABLEHANDLER_H_
#define _JSONTABLEHANDLER_H_

#include <QJsonDocument>
#include <QString>
#include <QTableWidget>

class JsonTableHandler {
 private:
  static const int kFlagIndex;
  static const int kRowIdIndex;

 public:
  JsonTableHandler(QTableWidget* table);
  void SaveJsonTable(const QString& path, const QString& fname);
  void SetJsonTable(const QString& path);

 private:
  bool GetTableCheckBoxState(int row, int column);
  void SetTableCheckBox(int row, int column, bool state);

  QTableWidget* table_;
};

#endif  // _JSONTABLEHANDLER_H_
