#ifndef _CREATEPATCHWORKER_H_
#define _CREATEPATCHWORKER_H_

#include <QObject>
#include <QString>

#include "QtGui/enchanttableworker.h"
#include "QtGui/mpqarchiver.h"
#include "QtGui/spelltableworker.h"
#include "QtGui/yserachanger.h"

class CreatePatchWorker : public QObject {
  Q_OBJECT

 public:
  explicit CreatePatchWorker(MPQArchiver* mpq_archiver,
                             SpellTableWorker* spell_table_worker,
                             EnchantTableWorker* enchant_table_worker,
                             QObject* parent = nullptr);

 public slots:
  void StartCreatingPatch();

 signals:
  void ProgressChanged(int value);
  void ErrorOccurred(const QString& error);

 private:
  MPQArchiver* mpq_archiver_;
  SpellTableWorker* spell_table_worker_;
  EnchantTableWorker* enchant_table_worker_;
  YseraChanger ysera_changer_;
};

#endif  // _CREATEPATCHWORKER_H_
