#include "dbc/dbcchanger.h"

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "dbc/dbchandler.h"

DBCChanger::DBCChanger() = default;

DBCChanger::~DBCChanger() = default;

bool DBCChanger::ChangeDBCFile(const std::string& dst_path,
                               const std::string& src_path,
                               const std::vector<ReplaceFields>& replace) {
  DBCHandler handler;
  if (handler.Load(src_path) != DBCError::kSuccess) {
    return false;
  }

  for (const auto& [record_id, fields] : replace) {
    Record record = handler.GetRecordById(record_id);
    for (const auto& [field, new_value] : fields) {
      record.SetUInt32(field, new_value);
    }
  }

  if (handler.Save(dst_path) != DBCError::kSuccess) {
    return false;
  }
  return true;
}
