#ifndef _JSONTABLEHANDLER_H_
#define _JSONTABLEHANDLER_H_

#include <QJsonDocument>
#include <QObject>
#include <QTableWidget>

class JsonTableHandler {
 private:
  const int kFlagIndex = 0;
  const int kRowIdIndex = 1;

 public:
  JsonTableHandler(QTableWidget* table);
  QJsonDocument GetJsonTable();
  void SetJsonTable(const QJsonDocument& json);

 private:
  bool GetTableCheckBoxState(int row, int column);
  void SetTableCheckBox(int row, int column, bool state);

  QTableWidget* table_;
};

#endif  // _JSONTABLEHANDLER_H_
