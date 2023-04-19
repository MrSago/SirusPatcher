#ifndef _ABSTRACTDBCTABLE_H_
#define _ABSTRACTDBCTABLE_H_

#include <QJsonArray>
#include <QString>
#include <QTableWidget>
#include <QWidget>

#include "DBC/dbchandler.h"

class AbstractDBCTable {
 public:
  virtual bool SetupTable(const QString& dbc_path,
                          const QString& json_path) = 0;
  ~AbstractDBCTable() = default;

 protected:
  AbstractDBCTable(QTableWidget* table) : table_(table) {}
  virtual void InitHeaders() = 0;
  virtual void InitFields(QJsonArray& ids) = 0;
  virtual QWidget* CreateCheckBox(bool default_state) = 0;

  QTableWidget* table_;
  DBCHandler handler_;
};

#endif  // _ABSTRACTDBCTABLE_H_
