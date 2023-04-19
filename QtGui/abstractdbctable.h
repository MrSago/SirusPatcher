#ifndef _ABSTRACTDBCTABLE_H_
#define _ABSTRACTDBCTABLE_H_

#include <QJsonArray>
#include <QString>
#include <QTableWidget>
#include <QWidget>

#include "DBC/dbchandler.h"

class AbstractDBCTable {
 public:
  virtual ~AbstractDBCTable() = default;
  virtual bool SetupTableFromFile() = 0;
  virtual bool WriteValuesToFile() = 0;

 protected:
  AbstractDBCTable(QTableWidget* table, const QString& dbc_path,
                   const QString& json_path)
      : table_(table), dbc_path_(dbc_path), json_path_(json_path) {}
  virtual void InitHeaders() = 0;
  virtual void InitFields(QJsonArray& ids) = 0;
  virtual QWidget* CreateCheckBox(bool default_state) = 0;

  QTableWidget* table_;
  QString dbc_path_;
  QString json_path_;
  DBCHandler handler_;
};

#endif  // _ABSTRACTDBCTABLE_H_
