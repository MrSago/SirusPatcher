#ifndef _SPELLDBCTABLE_H_
#define _SPELLDBCTABLE_H_

#include <QJsonArray>
#include <QString>
#include <QTableWidget>
#include <QWidget>

#include "DBC/dbchandler.h"
#include "QtGui/abstractdbctable.h"

class SpellDBCTable final : public AbstractDBCTable {
 public:
  SpellDBCTable(QTableWidget* table, const QString& dbc_path,
                const QString& json_path);
  bool SetupTableFromFile() override;
  bool WriteValuesToFile() override;

 private:
  void InitHeaders() override;
  void InitFields(QJsonArray& ids) override;
  QWidget* CreateCheckBox(bool default_state) override;
};

#endif  // _SPELLDBCTABLE_H_
