#ifndef _ENCHANTTABLEWORKER_H_
#define _ENCHANTTABLEWORKER_H_

#include <QObject>
#include <QString>
#include <QTableWidget>
#include <QVariant>

class EnchantTableWorker : public QObject {
  Q_OBJECT

 private:
  static const QString kDbcPath;
  static const QString kDbcSavePath;
  static const QString kJsonTablePath;

  static const int kItemVisual;
  static const int kRefName;

 public:
  explicit EnchantTableWorker(QTableWidget* table, QObject* parent = nullptr);

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

#endif  // _ENCHANTTABLEWORKER_H_
