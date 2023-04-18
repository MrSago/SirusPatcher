#ifndef _SPELLDBCTABLE_H_
#define _SPELLDBCTABLE_H_

#include <QCheckBox>
#include <QJsonArray>
#include <QString>
#include <QTableWidget>

#include "DBC/dbchandler.h"

class SpellDBCTable {
 public:
  SpellDBCTable(QTableWidget* table);
  bool SetupTable(const QString& dbc_path, const QString& json_path);

 private:
  void InitHeaders();
  void InitFields(DBCHandler& handler, QJsonArray& ids);
  QWidget* CreateCheckBox(bool default_state);

  QTableWidget* table_;
};

#endif  // _SPELLDBCTABLE_H_
