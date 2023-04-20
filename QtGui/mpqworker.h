#ifndef _MPQWORKER_H_
#define _MPQWORKER_H_

#include <QString>
#include <QStringList>

class MPQWorker {
 public:
  static const QString kImportPatchDirectory;
  static const QString kExportPatchDirectory;

  static bool ExtractDBCFiles(const QStringList& dbc_list,
                              const QString& game_path,
                              const QString& save_path);
  static bool ArchiveDBCFiles(const QStringList& dbc_list,
                              const QString& game_path);

 private:
  MPQWorker();
};

#endif  // _MPQWORKER_H_
