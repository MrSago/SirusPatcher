#include "QtGui/createpatchworker.h"

#include <QObject>

#include "QtGui/enchanttableworker.h"
#include "QtGui/mpqarchiver.h"
#include "QtGui/spelltableworker.h"

CreatePatchWorker::CreatePatchWorker(MPQArchiver* mpq_archiver,
                                     SpellTableWorker* spell_table_worker,
                                     EnchantTableWorker* enchant_table_worker,
                                     QObject* parent)
    : QObject(parent),
      mpq_archiver_(mpq_archiver),
      spell_table_worker_(spell_table_worker),
      enchant_table_worker_(enchant_table_worker) {}

void CreatePatchWorker::StartCreatingPatch() {
  bool success = spell_table_worker_->WriteDBCTable();
  if (!success) {
    emit ErrorOccurred("Ошибка при записи Spell.dbc");
    return;
  }

  success = enchant_table_worker_->WriteDBCTable();
  if (!success) {
    emit ErrorOccurred("Ошибка при записи SpellItemEnchantment.dbc");
    return;
  }

  success = mpq_archiver_->ArchiveDBCFiles();
  if (!success) {
    emit ErrorOccurred("Ошибка при архивировании DBC файлов");
    return;
  }
}
