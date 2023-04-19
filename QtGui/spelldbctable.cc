#include "QtGui/spelldbctable.h"

#include <QCheckBox>
#include <QFile>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QString>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>

#include "DBC/dbchandler.h"
#include "DBC/record.h"

SpellDBCTable::SpellDBCTable(QTableWidget* table, const QString& dbc_path,
                             const QString& json_path)
    : AbstractDBCTable(table, dbc_path, json_path) {}

bool SpellDBCTable::SetupTableFromFile() {
  if (handler_.Load(dbc_path_.toStdString()) != DBCError::kSuccess) {
    return false;
  }

  QFile::remove(dbc_path_);

  QFile json_file(json_path_);
  if (!json_file.open(QIODevice::ReadOnly)) return false;

  QJsonArray ids_array = QJsonDocument::fromJson(json_file.readAll()).array();

  InitHeaders();
  InitFields(ids_array);

  return true;
}

bool SpellDBCTable::WriteValuesToFile() {
  for (int i = 0; i < table_->rowCount(); ++i) {
    QWidget* item = (table_->cellWidget(i, 0));
    QCheckBox* check_box =
        qobject_cast<QCheckBox*>(item->layout()->itemAt(0)->widget());

    bool state = check_box->isChecked();
    int id = table_->item(i, 1)->data(Qt::DisplayRole).toInt();

    if (!state) {
      handler_.GetRecordById(id).SetUInt32(132, 0);
    }
  }

  if (handler_.Save(dbc_path_.toStdString()) != DBCError::kSuccess) {
    return false;
  }

  return true;
}

void SpellDBCTable::InitHeaders() {
  QStringList headers;
  headers << "*"
          << "ID"
          << "Название"
          << "Описание";
  table_->setColumnCount(headers.size());
  table_->setHorizontalHeaderLabels(headers);

  table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  table_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

  table_->setColumnWidth(0, 35);
  table_->setColumnWidth(1, 60);
  table_->setColumnWidth(2, 200);
  table_->setColumnWidth(3, 510);
}

void SpellDBCTable::InitFields(QJsonArray& ids) {
  table_->setRowCount(ids.count());
  for (int i = 0; i < ids.count(); ++i) {
    int id = ids[i].toInt();
    Record record = handler_.GetRecordById(id);

    table_->setCellWidget(i, 0, CreateCheckBox(true));

    QTableWidgetItem* id_item = new QTableWidgetItem();
    id_item->setData(Qt::DisplayRole, id);
    table_->setItem(i, 1, id_item);

    QTableWidgetItem* spell_name_item =
        new QTableWidgetItem(QString::fromUtf8(record.GetString(145)));
    table_->setItem(i, 2, spell_name_item);

    QTableWidgetItem* description_item =
        new QTableWidgetItem(QString::fromUtf8(record.GetString(179)));
    table_->setItem(i, 3, description_item);
  }

  table_->sortItems(2, Qt::AscendingOrder);
}

QWidget* SpellDBCTable::CreateCheckBox(bool default_state) {
  QWidget* widget = new QWidget();
  QCheckBox* check_box = new QCheckBox();
  QHBoxLayout* layout = new QHBoxLayout(widget);
  layout->addWidget(check_box);
  layout->setAlignment(Qt::AlignCenter);
  layout->setContentsMargins(0, 0, 0, 0);
  check_box->setChecked(default_state);
  return widget;
}
