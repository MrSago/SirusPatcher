#include "QtGui/spelltableworker.h"

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

const QString SpellTableWorker::kDbcPath = "./Spell.dbc";
const QString SpellTableWorker::kDbcSavePath = "./Spell.dbc.save";
const QString SpellTableWorker::kJsonTablePath =
    "://resources/dbc/Spell.dbc.json";

constexpr int SpellTableWorker::kSpellVisual = 132;
constexpr int SpellTableWorker::kSpellName = 145;
constexpr int SpellTableWorker::kSpellDescription = 179;

SpellTableWorker::SpellTableWorker(QTableWidget* table, QObject* parent)
    : QObject(parent), table_(table) {}

bool SpellTableWorker::InitDBCTable() {
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
      emit AddItem(table_, index, 1, id);
      emit AddItem(table_, index, 2,
                   QString::fromUtf8(record.GetString(kSpellName)));
      emit AddItem(table_, index, 3,
                   QString::fromUtf8(record.GetString(kSpellDescription)));
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

bool SpellTableWorker::WriteDBCTable() {
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
      handler.GetRecordById(id).SetUInt32(kSpellVisual, 0);
    }

    emit AddProgressBarValue(1);
  }

  if (handler.Save(kDbcPath.toStdString()) != DBCError::kSuccess) {
    emit ErrorOccurred("Ошибка сохранения DBC файла:\n" + kDbcPath);
    return false;
  }

  return true;
}
