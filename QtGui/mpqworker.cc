#include "QtGui/mpqworker.h"

#include <QFile>
#include <QString>
#include <QStringList>
#include <string>

#include "external/include/StormLib.h"

const QString MPQWorker::kImportPatchDirectory = "Data/ruRU/patch-ruRU-4.mpq";
const QString MPQWorker::kExportPatchDirectory = "Data/ruRU/patch-ruRU-].mpq";

bool MPQWorker::ExtractDBCFiles(const QStringList& dbc_list,
                                const QString& game_path,
                                const QString& save_path) {
  std::string imported_mpq_string =
      (game_path + '/' + kImportPatchDirectory).toStdString();
  const TCHAR* imported_mpq_pointer =
      reinterpret_cast<const TCHAR*>(imported_mpq_string.c_str());

  HANDLE mpq;
  bool success =
      SFileOpenArchive(imported_mpq_pointer, 0, STREAM_FLAG_READ_ONLY, &mpq);
  if (!success) return false;

  for (auto& file : dbc_list) {
    std::string dbc_file_path_string = ("DBFilesClient\\" + file).toStdString();
    const char* dbc_file_path_pointer = dbc_file_path_string.c_str();

    std::string dbc_exported_path_string =
        (save_path + "/" + file + ".save").toStdString();
    const TCHAR* dbc_exported_path_pointer =
        reinterpret_cast<const TCHAR*>(dbc_exported_path_string.c_str());

    success = SFileExtractFile(mpq, dbc_file_path_pointer,
                               dbc_exported_path_pointer, SFILE_OPEN_FROM_MPQ);
    if (!success) return false;
  }

  return SFileCloseArchive(mpq);
}

bool MPQWorker::ArchiveDBCFiles(const QStringList& dbc_files,
                                const QString& game_dir) {
  bool success = QFile::remove(game_dir + '/' + kExportPatchDirectory);
  if (!success) return false;

  std::string mpq_exported_path_string =
      (game_dir + '/' + kExportPatchDirectory).toStdString();
  const TCHAR* mpq_exported_path_pointer =
      reinterpret_cast<const TCHAR*>(mpq_exported_path_string.c_str());

  HANDLE mpq;
  success = SFileCreateArchive(mpq_exported_path_pointer,
                               MPQ_CREATE_ATTRIBUTES + MPQ_CREATE_ARCHIVE_V2,
                               dbc_files.size() + 2, &mpq);
  if (!success) return false;

  for (auto& file : dbc_files) {
    std::string dbc_exported_path_string = file.toStdString();
    const TCHAR* dbc_exported_path_pointer =
        reinterpret_cast<const TCHAR*>(dbc_exported_path_string.c_str());

    std::string dbc_file_string = ("DBFilesClient\\" + file).toStdString();
    const char* dbc_file_pointer = dbc_file_string.c_str();

    success = SFileAddFileEx(mpq, dbc_exported_path_pointer, dbc_file_pointer,
                             MPQ_FILE_COMPRESS + MPQ_FILE_REPLACEEXISTING,
                             MPQ_COMPRESSION_ZLIB, MPQ_COMPRESSION_NEXT_SAME);
    success &= QFile::remove(file);
    if (!success) return false;
  }

  return SFileCloseArchive(mpq);
}

MPQWorker::MPQWorker() = default;
