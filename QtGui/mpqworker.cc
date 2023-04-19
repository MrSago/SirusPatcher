#include "mpqworker.h"

#include <QDir>
#include <QFile>
#include <QString>
#include <QStringList>

#include "external/include/StormLib.h"

const QString MPQWorker::kImportPatchDirectory = "Data/ruRU/patch-ruRU-4.mpq";
const QString MPQWorker::kExportPatchDirectory = "Data/ruRU/patch-ruRU-].mpq";

bool MPQWorker::ExtractDBCFiles(const QStringList& files,
                                const QString& game_dir,
                                const QString& save_dir) {
  std::string imported_mpq_string =
      (game_dir + '/' + kImportPatchDirectory).toStdString();
  const TCHAR* imported_mpq_pointer =
      reinterpret_cast<const TCHAR*>(imported_mpq_string.c_str());

  HANDLE mpq;
  bool success =
      SFileOpenArchive(imported_mpq_pointer, 0, STREAM_FLAG_READ_ONLY, &mpq);
  if (!success) return false;

  for (auto& file : files) {
    std::string file_string = ("DBFilesClient\\" + file).toStdString();
    const char* file_pointer = file_string.c_str();

    std::string exported_dbc_string = (save_dir + "/" + file).toStdString();
    const TCHAR* exported_dbc_pointer =
        reinterpret_cast<const TCHAR*>(exported_dbc_string.c_str());

    success = SFileExtractFile(mpq, file_pointer, exported_dbc_pointer,
                               SFILE_OPEN_FROM_MPQ);
    if (!success) return false;
  }

  return SFileCloseArchive(mpq);
}

bool MPQWorker::ArchiveDBCFiles(const QStringList& files,
                                const QString& game_dir,
                                const QString& save_dir) {
  QFile::remove(game_dir + '/' + kExportPatchDirectory);

  std::string exported_mpq_string =
      (game_dir + '/' + kExportPatchDirectory).toStdString();
  const TCHAR* exported_mpq_pointer =
      reinterpret_cast<const TCHAR*>(exported_mpq_string.c_str());

  HANDLE mpq;
  bool success = SFileCreateArchive(
      exported_mpq_pointer, MPQ_CREATE_ATTRIBUTES + MPQ_CREATE_ARCHIVE_V2,
      files.size() + 1, &mpq);
  if (!success) return false;

  for (auto& file : files) {
    std::string exported_dbc = file.toStdString();
    const TCHAR* exported_dbc_pointer =
        reinterpret_cast<const TCHAR*>(exported_dbc.c_str());

    std::string dbc_file = ("DBFilesClient\\" + file).toStdString();
    const char* dbc_file_pointer = dbc_file.c_str();

    success = SFileAddFileEx(mpq, exported_dbc_pointer, dbc_file_pointer,
                             MPQ_FILE_COMPRESS + MPQ_FILE_REPLACEEXISTING,
                             MPQ_COMPRESSION_ZLIB, MPQ_COMPRESSION_NEXT_SAME);

    success &= QFile::remove(file);

    if (!success) return false;
  }

  return SFileCloseArchive(mpq);
}

MPQWorker::MPQWorker() = default;
