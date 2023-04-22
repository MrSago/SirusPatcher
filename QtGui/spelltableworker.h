#ifndef _SPELLTABLEWORKER_H_
#define _SPELLTABLEWORKER_H_

#include <QObject>
#include <QString>
#include <QTableWidget>
#include <QVariant>

#include "DBC/dbchandler.h"

class SpellTableWorker : public QObject {
  Q_OBJECT

 private:
  static const QString kDbcPath;
  static const QString kDbcSavePath;
  static const QString kJsonTablePath;

  static const int kSpellVisualColumn;
  static const int kSpellNameColumn;
  static const int kSpellDescriptionColumn;

 public:
  explicit SpellTableWorker(QTableWidget* table, QObject* parent = nullptr);

 public slots:
  bool InitDBCTable();
  bool WriteDBCTable();

 signals:
  void ResetRowCount(QTableWidget* table, int count);
  void AddItem(QTableWidget* table, int row, int column, const QVariant& text);
  void ProgressChanged(int value);
  void ErrorOccurred(const QString& error);

 private:
  QTableWidget* table_;
};

#endif  // _SPELLTABLEWORKER_H_
