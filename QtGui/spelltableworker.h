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

  static const int kSpellVisual;
  static const int kSpellName;
  static const int kSpellDescription;

 public:
  explicit SpellTableWorker(QTableWidget* table, QObject* parent = nullptr);

 public slots:
  bool InitDBCTable();
  bool WriteDBCTable();

 signals:
  void SetRowCount(QTableWidget* table, int count);
  void ResetRowCount(QTableWidget* table, int count);
  void AddItem(QTableWidget* table, int row, int column, const QVariant& text);
  void AddProgressBarValue(int value);
  void SetProgressBarMaximum(int maximum);
  void AddTotalRecords(int count);
  void ErrorOccurred(const QString& error);
  void WarningOccurred(const QString& warning);

 private:
  QTableWidget* table_;
};

#endif  // _SPELLTABLEWORKER_H_
