#ifndef _SPELLDBCTABLE_H_
#define _SPELLDBCTABLE_H_

#include <QDir>
#include <QJsonArray>
#include <QObject>
#include <QString>
#include <QTableWidget>
#include <QWidget>

#include "DBC/dbchandler.h"

class SpellDBCTable final : public QObject {
  Q_OBJECT

 public:
  SpellDBCTable(QTableWidget* table);
  ~SpellDBCTable();
  bool SetupFromFile();
  bool WriteValuesToFile();
  void SetTableCheckBoxes(bool state);

 signals:
  void UpdateProgress(int progress);

 private:
  const QString kDbcPath = "./Spell.dbc";
  const QString kDbcSavePath = "./Spell.dbc.save";
  const QString kJsonTablePath = "://resources/dbc/Spell.dbc.json";

  const int kSpellVisualColumn = 132;
  const int kSpellNameColumn = 145;
  const int kSpellDescriptionColumn = 179;

  void InitHeaders();
  void InitFields(QJsonArray& ids, DBCHandler& handler);
  QWidget* CreateCheckBox(bool default_state);

  QTableWidget* table_;
};

#endif  // _SPELLDBCTABLE_H_
