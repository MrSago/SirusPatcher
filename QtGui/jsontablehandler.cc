#include "jsontablehandler.h"

#include <QCheckBox>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLayout>
#include <QString>
#include <QTableWidget>
#include <QWidget>

constexpr int JsonTableHandler::kFlagIndex = 0;
constexpr int JsonTableHandler::kRowIdIndex = 1;

JsonTableHandler::JsonTableHandler(QTableWidget* table) : table_(table) {}

void JsonTableHandler::SaveJsonTable(const QString& path,
                                     const QString& fname) {
  QJsonObject rows;
  for (int i = 0; i < table_->rowCount(); ++i) {
    bool flag = GetTableCheckBoxState(i, kFlagIndex);
    QString id = table_->item(i, kRowIdIndex)->text();
    rows[id] = flag;
  }
  QJsonDocument doc = QJsonDocument(rows);

  if (!QDir(path).exists()) {
    QDir().mkdir(path);
  }
  QFile file(fname);
  if (!file.open(QIODevice::WriteOnly)) {
    return;
  }
  file.write(doc.toJson());
  file.close();
}

void JsonTableHandler::SetJsonTable(const QString& fname) {
  QFile file(fname);
  if (!file.open(QIODevice::ReadOnly)) {
    return;
  }
  QJsonDocument json = QJsonDocument::fromJson(file.readAll());
  file.close();

  QJsonObject rows = json.object();

  table_->setSortingEnabled(false);

  QHash<QString, int> row_lookup;
  for (int i = 0; i < table_->rowCount(); ++i) {
    QString id = table_->item(i, kRowIdIndex)->text();
    row_lookup[id] = i;
  }

  for (QJsonObject::iterator it = rows.begin(); it != rows.end(); ++it) {
    QString id = it.key();
    bool flag = it.value().toBool();

    if (row_lookup.contains(id)) {
      SetTableCheckBox(row_lookup[id], kFlagIndex, flag);
    }
  }

  table_->setSortingEnabled(true);
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
