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

SpellDBCTable::SpellDBCTable(QTableWidget* table) : table_(table) {}

SpellDBCTable::~SpellDBCTable() { table_->clear(); }

bool SpellDBCTable::SetupFromFile() {
  table_->clear();

  DBCHandler handler;
  if (handler.Load(kDbcSavePath.toStdString()) != DBCError::kSuccess) {
    return false;
  }

  QFile json_file(kJsonTablePath);
  if (!json_file.open(QIODevice::ReadOnly)) {
    return false;
  }

  QJsonArray ids_array = QJsonDocument::fromJson(json_file.readAll()).array();

  InitHeaders();
  InitFields(ids_array, handler);

  return true;
}

bool SpellDBCTable::WriteValuesToFile() {
  DBCHandler handler;
  if (handler.Load(kDbcSavePath.toStdString()) != DBCError::kSuccess) {
    return false;
  }

  for (int i = 0; i < table_->rowCount(); ++i) {
    QWidget* item = (table_->cellWidget(i, 0));
    QCheckBox* check_box =
        qobject_cast<QCheckBox*>(item->layout()->itemAt(0)->widget());

    bool state = check_box->isChecked();
    int id = table_->item(i, 1)->data(Qt::DisplayRole).toInt();

    if (!state) {
      handler.GetRecordById(id).SetUInt32(kSpellVisualColumn, 0);
    }
  }

  if (handler.Save(kDbcPath.toStdString()) != DBCError::kSuccess) {
    return false;
  }

  return true;
}

void SpellDBCTable::SetTableCheckBoxes(bool state) {
  for (int i = 0; i < table_->rowCount(); ++i) {
    QWidget* item = table_->cellWidget(i, 0);
    QCheckBox* check_box =
        qobject_cast<QCheckBox*>(item->layout()->itemAt(0)->widget());
    check_box->setChecked(state);
  }
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

void SpellDBCTable::InitFields(QJsonArray& ids, DBCHandler& handler) {
  table_->setRowCount(ids.count());
  for (int i = 0; i < ids.count(); ++i) {
    int id = ids[i].toInt();
    Record record = handler.GetRecordById(id);

    table_->setCellWidget(i, 0, CreateCheckBox(true));

    QTableWidgetItem* id_item = new QTableWidgetItem();
    id_item->setData(Qt::DisplayRole, id);
    table_->setItem(i, 1, id_item);

    QTableWidgetItem* spell_name_item = new QTableWidgetItem(
        QString::fromUtf8(record.GetString(kSpellNameColumn)));
    table_->setItem(i, 2, spell_name_item);

    QTableWidgetItem* description_item = new QTableWidgetItem(
        QString::fromUtf8(record.GetString(kSpellDescriptionColumn)));
    table_->setItem(i, 3, description_item);
  }

  table_->sortItems(2, Qt::AscendingOrder);
}

QWidget* SpellDBCTable::CreateCheckBox(bool state) {
  QWidget* widget = new QWidget();
  QCheckBox* check_box = new QCheckBox();
  QHBoxLayout* layout = new QHBoxLayout(widget);
  layout->addWidget(check_box);
  layout->setAlignment(Qt::AlignCenter);
  layout->setContentsMargins(0, 0, 0, 0);
  check_box->setChecked(state);
  return widget;
}
