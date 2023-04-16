#ifndef _DBC_CREATOR_H
#define _DBC_CREATOR_H

#include <cstdint>
#include <map>
#include <vector>

#include "dbc/dbchandler.h"

struct Field {
  uint32_t field;
  uint32_t new_value;
};

struct ReplaceFields {
  uint32_t record_id;
  std::vector<Field> fields;
};

class DBCChanger {
 public:
  DBCChanger();
  ~DBCChanger();

  bool ChangeDBCFile(const std::string& dst_path, const std::string& src_path,
                     const std::vector<ReplaceFields>& replace);
};

#endif  // _DBC_CREATOR_H
