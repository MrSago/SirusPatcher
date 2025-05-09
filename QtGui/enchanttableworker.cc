#include "QtGui/enchanttableworker.h"

#include <QCheckBox>
#include <QFile>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLayout>
#include <QObject>
#include <QString>
#include <QTableWidget>
#include <QWidget>

#include "DBC/dbchandler.h"
#include "DBC/record.h"

const QString EnchantTableWorker::kDbcPath = "./SpellItemEnchantment.dbc";
const QString EnchantTableWorker::kDbcSavePath =
    "./SpellItemEnchantment.dbc.save";
const QString EnchantTableWorker::kJsonTablePath =
    "./dbc/SpellItemEnchantment.dbc.json";

constexpr int EnchantTableWorker::kItemVisual = 32;
constexpr int EnchantTableWorker::kRefName = 23;

EnchantTableWorker::EnchantTableWorker(QTableWidget* table, QObject* parent)
    : QObject(parent), table_(table) {}

bool EnchantTableWorker::InitDBCTable() {
  DBCHandler handler;
  if (handler.Load(kDbcSavePath.toStdString()) != DBCError::kSuccess) {
    emit ErrorOccurred("Ошибка загрузки DBC файла:\n" + kDbcSavePath);
    return false;
  }

  QFile json_file(kJsonTablePath);
  if (!json_file.open(QIODevice::ReadOnly)) {
    emit ErrorOccurred("Ошибка открытия JSON файла:\n" + kJsonTablePath);
    return false;
  }

  QJsonArray ids_array = QJsonDocument::fromJson(json_file.readAll()).array();

  emit AddTotalRecords(ids_array.count());

  emit ResetRowCount(table_, ids_array.count());

  int skipped_index = 0;
  for (int i = 0; i < ids_array.count(); ++i) {
    int id = ids_array[i].toInt();
    int index = i - skipped_index;

    try {
      Record record = handler.GetRecordById(id);
      emit AddItem(table_, i, 1, id);
      QString ref_name =
          QString::fromUtf8(record.GetString(kRefName)).sliced(10).chopped(2);
      emit AddItem(table_, i, 2, ref_name);
    } catch (const std::exception& e) {
      ++skipped_index;
      emit WarningOccurred(QString("ID %1 из файла %2 не найден\n%3")
                               .arg(QString::number(id), kDbcPath, e.what()));
    }

    emit AddProgressBarValue(1);
  }

  emit SetRowCount(table_, ids_array.count() - skipped_index);

  return true;
}

bool EnchantTableWorker::WriteDBCTable() {
  DBCHandler handler;
  if (handler.Load(kDbcSavePath.toStdString()) != DBCError::kSuccess) {
    emit ErrorOccurred("Ошибка загрузки DBC файла:\n" + kDbcSavePath);
    return false;
  }

  emit AddTotalRecords(table_->rowCount());

  for (int i = 0; i < table_->rowCount(); ++i) {
    QWidget* item = (table_->cellWidget(i, 0));
    QCheckBox* check_box =
        qobject_cast<QCheckBox*>(item->layout()->itemAt(0)->widget());

    bool state = check_box->isChecked();
    int id = table_->item(i, 1)->data(Qt::DisplayRole).toInt();

    if (!state) {
      handler.GetRecordById(id).SetUInt32(kItemVisual, 0);
    }

    emit AddProgressBarValue(1);
  }

  if (handler.Save(kDbcPath.toStdString()) != DBCError::kSuccess) {
    emit ErrorOccurred("Ошибка сохранения DBC файла:\n" + kDbcPath);
    return false;
  }

  return true;
}
