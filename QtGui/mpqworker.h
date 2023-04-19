#ifndef _MPQWORKER_H_
#define _MPQWORKER_H_

#include <QString>
#include <QStringList>

class MPQWorker {
 public:
  static const QString kImportPatchDirectory;
  static const QString kExportPatchDirectory;

  static bool ExtractDBCFiles(const QStringList& files, const QString& game_dir,
                              const QString& save_dir);
  static bool ArchiveDBCFiles(const QStringList& files,
                              const QString& game_dir);

 private:
  MPQWorker();
};

#endif  // _MPQWORKER_H_
