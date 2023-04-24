#include "QtGui/mpqarchiver.h"

#include <QFile>
#include <QObject>
#include <QString>
#include <QStringList>
#include <string>

#include "external/include/StormLib.h"

const QString MPQArchiver::kImportPatchDirectory = "Data/ruRU/patch-ruRU-4.mpq";
const QString MPQArchiver::kExportPatchDirectory = "Data/ruRU/patch-ruRU-].mpq";

MPQArchiver::MPQArchiver(QObject* parent) : QObject(parent) {}

void MPQArchiver::SetParameters(const QStringList& dbc_list,
                                const QString& game_path,
                                const QString& save_path) {
  dbc_list_ = dbc_list;
  game_path_ = game_path;
  save_path_ = save_path;
}

bool MPQArchiver::ExtractDBCFiles(const QStringList& dbc_list,
                                  const QString& game_path,
                                  const QString& save_path) {
  std::string imported_mpq_string =
      (game_path + '/' + kImportPatchDirectory).toStdString();
  const TCHAR* imported_mpq_pointer =
      reinterpret_cast<const TCHAR*>(imported_mpq_string.c_str());

  HANDLE mpq;
  bool success =
      SFileOpenArchive(imported_mpq_pointer, 0, STREAM_FLAG_READ_ONLY, &mpq);
  if (!success) {
    emit ErrorOccurred("Не удалось открыть MPQ архив:\n" +
                       kImportPatchDirectory);
    return false;
  }

  for (auto& file : dbc_list) {
    std::string dbc_file_path_string = ("DBFilesClient\\" + file).toStdString();
    const char* dbc_file_path_pointer = dbc_file_path_string.c_str();

    std::string dbc_exported_path_string =
        (save_path + "/" + file + ".save").toStdString();
    const TCHAR* dbc_exported_path_pointer =
        reinterpret_cast<const TCHAR*>(dbc_exported_path_string.c_str());

    success = SFileExtractFile(mpq, dbc_file_path_pointer,
                               dbc_exported_path_pointer, SFILE_OPEN_FROM_MPQ);
    if (!success) {
      emit ErrorOccurred("Не удалось извлечь DBC файл:\n" + file);
      return false;
    }
  }

  success = SFileCloseArchive(mpq);
  if (!success) {
    emit ErrorOccurred("Не удалось закрыть MPQ архив:\n" +
                       kImportPatchDirectory);
    return false;
  }

  return true;
}

bool MPQArchiver::ArchiveDBCFiles(const QStringList& dbc_files,
                                  const QString& game_path) {
  QFile::remove(game_path + '/' + kExportPatchDirectory);

  std::string mpq_exported_path_string =
      (game_path + '/' + kExportPatchDirectory).toStdString();
  const TCHAR* mpq_exported_path_pointer =
      reinterpret_cast<const TCHAR*>(mpq_exported_path_string.c_str());

  HANDLE mpq;
  bool success = SFileCreateArchive(
      mpq_exported_path_pointer, MPQ_CREATE_ATTRIBUTES + MPQ_CREATE_ARCHIVE_V2,
      dbc_files.size() + 2, &mpq);
  if (!success) {
    emit ErrorOccurred("Не удалось создать MPQ архив:\n" +
                       kExportPatchDirectory);
    return false;
  }

  for (auto& file : dbc_files) {
    std::string dbc_exported_path_string = file.toStdString();
    const TCHAR* dbc_exported_path_pointer =
        reinterpret_cast<const TCHAR*>(dbc_exported_path_string.c_str());

    std::string dbc_file_string = ("DBFilesClient\\" + file).toStdString();
    const char* dbc_file_pointer = dbc_file_string.c_str();

    success = SFileAddFileEx(mpq, dbc_exported_path_pointer, dbc_file_pointer,
                             MPQ_FILE_COMPRESS + MPQ_FILE_REPLACEEXISTING,
                             MPQ_COMPRESSION_ZLIB, MPQ_COMPRESSION_NEXT_SAME);

    success |= QFile::remove(file);

    if (!success) {
      emit ErrorOccurred("Не удалось добавить DBC файл в MPQ архив:\n" + file);
      return false;
    }
  }

  success = SFileCloseArchive(mpq);
  if (!success) {
    emit ErrorOccurred("Не удалось закрыть MPQ архив:\n" +
                       kExportPatchDirectory);
    return false;
  }

  return true;
}

bool MPQArchiver::ExtractDBCFiles() {
  if (dbc_list_.isEmpty() || game_path_.isEmpty() || save_path_.isEmpty()) {
    emit ErrorOccurred("Не заданы параметры для извлечения DBC файлов");
    return false;
  }

  return ExtractDBCFiles(dbc_list_, game_path_, save_path_);
}

bool MPQArchiver::ArchiveDBCFiles() {
  if (dbc_list_.isEmpty() || game_path_.isEmpty()) {
    emit ErrorOccurred("Не заданы параметры для архивации DBC файлов");
    return false;
  }

  return ArchiveDBCFiles(dbc_list_, game_path_);
}
