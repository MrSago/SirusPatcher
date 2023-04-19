#include "spelldbctable.h"

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

SpellDBCTable::SpellDBCTable(QTableWidget* table) : AbstractDBCTable(table) {}

bool SpellDBCTable::SetupTable(const QString& dbc_path,
                               const QString& json_path) {
  if (handler_.Load(dbc_path.toStdString()) != DBCError::kSuccess) {
    return false;
  }

  QFile json_file(json_path);
  if (!json_file.open(QIODevice::ReadOnly)) return false;

  QJsonArray ids_array = QJsonDocument::fromJson(json_file.readAll()).array();

  InitHeaders();
  InitFields(ids_array);

  return true;
}

void SpellDBCTable::InitHeaders() {
  table_->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  table_->setColumnCount(3);
  QStringList headers;
  headers << "*"
          << "ID"
          << "Описание заклинания";
  table_->setHorizontalHeaderLabels(headers);
}

void SpellDBCTable::InitFields(QJsonArray& ids) {
  table_->setRowCount(ids.count());
  for (int i = 0; i < ids.count(); ++i) {
    int id = ids[i].toInt();
    table_->setCellWidget(i, 0, CreateCheckBox(true));

    QTableWidgetItem* id_item = new QTableWidgetItem(QString::number(id));
    table_->setItem(i, 1, id_item);

    QTableWidgetItem* description_item = new QTableWidgetItem(
        QString::fromUtf8(handler_.GetRecordById(id).GetString(145)));
    table_->setItem(i, 2, description_item);
  }
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
