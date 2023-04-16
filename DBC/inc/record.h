#ifndef _RECORD_H_
#define _RECORD_H_

#include <cstdint>
#include <string>

#include "dbc.h"

class DBCHandler;
struct DBCFile;

class Record {
 public:
  float GetFloat(uint32_t field) const;
  void SetFloat(uint32_t field, float val) const;

  uint32_t GetUInt32(uint32_t field) const;
  void SetUInt32(uint32_t field, uint32_t val) const;

  std::string GetString(uint32_t field) const;
  void SetStringRef(uint32_t field, const std::string& str);
  void AddStringRef(uint32_t field, const std::string& str);

 private:
  Record(DBCFile* dbc, uint32_t index);

  void ValidateIndex(uint32_t field) const;

  DBCFile* dbc_;
  char* pointer_;

  friend class DBCHandler;
};

#endif  // _RECORD_H_
