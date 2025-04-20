#ifndef _CREATEPATCHWORKER_H_
#define _CREATEPATCHWORKER_H_

#include <QObject>
#include <QString>

#include "QtGui/enchanttableworker.h"
#include "QtGui/ladyspiritschanger.h"
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
  void AddProgressBarValue(int value);
  void SetProgressBarMaximum(int maximum);
  void AddTotalRecords(int count);
  void ErrorOccurred(const QString& error);

 private:
  MPQArchiver* mpq_archiver_;
  SpellTableWorker* spell_table_worker_;
  EnchantTableWorker* enchant_table_worker_;
  YseraChanger ysera_changer_;
  LadySpiritsChanger lady_spirits_changer_;
};

#endif  // _CREATEPATCHWORKER_H_
