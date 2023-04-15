
#ifndef _DBC_H_
#define _DBC_H_

#include <cstdint>
#include <string>
#include <map>

#include "record.h"

class Record;

enum class DBCError {
  kSuccess = 0,
  kFileNotOpened,
  kFileOpenError,
  kInvalidMagic,
};

using stringref = uint32_t;

struct DBCHeader {
  uint32_t magic;
  uint32_t record_count;
  uint32_t field_count;
  uint32_t record_size;
  uint32_t string_block_size;
};

struct DBCFile {
  DBCHeader header;
  char* records;
  char* string_block;
};

class DBCHandler {
 public:
  DBCHandler();
  ~DBCHandler();

  DBCError Load(const std::string& filename);
  DBCError Save(const std::string& filename);
  void Clear();

  Record operator[](uint32_t index);
  Record GetRecordById(uint32_t record_id);
  Record AllocateNewRecord();

 private:
  const uint32_t kDBCMagic = 0x43424457;  // 'WDBC'

  DBCFile* dbc_;
  std::map<uint32_t, Record> record_id_map_;
};

#endif  // _DBC_H_
