#include "QtGui/createpatchworker.h"

#include <QObject>

#include "QtGui/enchanttableworker.h"
#include "QtGui/ladyspiritschanger.h"
#include "QtGui/mpqarchiver.h"
#include "QtGui/spelltableworker.h"
#include "QtGui/yserachanger.h"

CreatePatchWorker::CreatePatchWorker(MPQArchiver* mpq_archiver,
                                     SpellTableWorker* spell_table_worker,
                                     EnchantTableWorker* enchant_table_worker,
                                     QObject* parent)
    : QObject(parent),
      mpq_archiver_(mpq_archiver),
      spell_table_worker_(spell_table_worker),
      enchant_table_worker_(enchant_table_worker) {}

void CreatePatchWorker::StartCreatingPatch() {
  if (!spell_table_worker_->WriteDBCTable()) {
    emit ErrorOccurred("Ошибка при записи Spell.dbc");
    return;
  }

  if (!enchant_table_worker_->WriteDBCTable()) {
    emit ErrorOccurred("Ошибка при записи SpellItemEnchantment.dbc");
    return;
  }

  ysera_changer_.Change();
  lady_spirits_changer_.Change();

  if (!mpq_archiver_->ArchiveDBCFiles()) {
    emit ErrorOccurred("Ошибка при архивировании DBC файлов");
    return;
  }
}
