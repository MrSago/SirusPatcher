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

const int SpellTableWorker::kSpellVisual = 132;
const int SpellTableWorker::kSpellName = 145;
const int SpellTableWorker::kSpellDescription = 179;

SpellTableWorker::SpellTableWorker(QTableWidget* table, QObject* parent)
    : QObject(parent), table_(table) {}

bool SpellTableWorker::InitDBCTable() {
  DBCHandler handler;
  if (handler.Load(kDbcSavePath.toStdString()) != DBCError::kSuccess) {
    emit ErrorOccurred("Ошибка загрузки DBC файла:\n" + kDbcSavePath);
    return false;
  }
  emit ProgressChanged(3);

  QFile json_file(kJsonTablePath);
  if (!json_file.open(QIODevice::ReadOnly)) {
    emit ErrorOccurred("Ошибка открытия JSON файла:\n" + kJsonTablePath);
    return false;
  }

  QJsonArray ids_array = QJsonDocument::fromJson(json_file.readAll()).array();
  emit ProgressChanged(2);

  constexpr double kMaxProgress = 20.0l;
  const double kProgressStep = kMaxProgress / ids_array.count();
  double progress_double = 0.0l;
  int progress = 0;

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

    progress_double += kProgressStep;
    int current_progress = static_cast<int>(progress_double);
    if (current_progress != progress) {
      progress = current_progress;
      emit ProgressChanged(1);
    }
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

  constexpr double kMaxProgress = 33.0l;
  const double kProgressStep = kMaxProgress / table_->rowCount();
  double progress_double = 0.0l;
  int progress = 0;

  for (int i = 0; i < table_->rowCount(); ++i) {
    QWidget* item = (table_->cellWidget(i, 0));
    QCheckBox* check_box =
        qobject_cast<QCheckBox*>(item->layout()->itemAt(0)->widget());

    bool state = check_box->isChecked();
    int id = table_->item(i, 1)->data(Qt::DisplayRole).toInt();

    if (!state) {
      handler.GetRecordById(id).SetUInt32(kSpellVisual, 0);
    }

    progress_double += kProgressStep;
    int current_progress = int(progress_double);
    if (current_progress != progress) {
      progress = current_progress;
      emit ProgressChanged(1);
    }
  }

  if (handler.Save(kDbcPath.toStdString()) != DBCError::kSuccess) {
    emit ErrorOccurred("Ошибка сохранения DBC файла:\n" + kDbcPath);
    return false;
  }

  return true;
}
