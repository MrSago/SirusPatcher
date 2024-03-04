#include "jsontablehandler.h"

#include <QCheckBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLayout>
#include <QTableWidget>
#include <QWidget>

JsonTableHandler::JsonTableHandler(QTableWidget* table) : table_(table) {}

QJsonDocument JsonTableHandler::GetJsonTable() {
  QJsonObject rows;
  for (int i = 0; i < table_->rowCount(); ++i) {
    bool flag = GetTableCheckBoxState(i, kFlagIndex);
    QString id = table_->item(i, kRowIdIndex)->text();
    rows[id] = flag;
  }
  return QJsonDocument(rows);
}

void JsonTableHandler::SetJsonTable(const QJsonDocument& json) {
  QJsonObject rows = json.object();
  for (QJsonObject::iterator it = rows.begin(); it != rows.end(); ++it) {
    QString id = it.key();
    bool flag = it.value().toBool();
    // QTableWidgetItem* item = table->findItems(id, Qt::MatchExactly).first();
    // SetTableCheckBox(table, row, kFlagIndex, flag);
  }
}

bool JsonTableHandler::GetTableCheckBoxState(int row, int column) {
  QWidget* item = table_->cellWidget(row, column);
  QCheckBox* check_box =
      qobject_cast<QCheckBox*>(item->layout()->itemAt(0)->widget());
  return check_box->isChecked();
}

void JsonTableHandler::SetTableCheckBox(int row, int column, bool state) {
  QWidget* item = table_->cellWidget(row, column);
  QCheckBox* check_box =
      qobject_cast<QCheckBox*>(item->layout()->itemAt(0)->widget());
  check_box->setChecked(state);
}
