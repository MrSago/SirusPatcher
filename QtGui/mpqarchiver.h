#ifndef _MPQARCHIVER_H_
#define _MPQARCHIVER_H_

#include <QObject>
#include <QString>
#include <QStringList>

class MPQArchiver : public QObject {
  Q_OBJECT
 public:
  static const QString kImportPatchDirectory;
  static const QString kExportPatchDirectory;

 public:
  explicit MPQArchiver(QObject* parent = nullptr);
  void SetParameters(const QStringList& dbc_list, const QString& game_path,
                     const QString& save_path);

  bool ExtractDBCFiles(const QStringList& dbc_list, const QString& game_path,
                       const QString& save_path);
  bool ArchiveDBCFiles(const QStringList& dbc_list, const QString& game_path);

 public slots:
  bool ExtractDBCFiles();
  bool ArchiveDBCFiles();

 signals:
  void ProgressChanged(int value);
  void ErrorOccurred(const QString& error);

 private:
  QStringList dbc_list_;
  QString game_path_;
  QString save_path_;
};

#endif  // _MPQARCHIVER_H_
