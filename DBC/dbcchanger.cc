#include "dbc/dbcchanger.h"

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "dbc/dbchandler.h"

DBCChanger::DBCChanger() {
  dbc_method_map_[DBCTableType::kSpell] = &DBCChanger::ChangeSpellDBC;
}

DBCChanger::~DBCChanger() = default;

void DBCChanger::ChangeDBCFile(const std::string& dst_path,
                               const std::string& src_path,
                               const std::vector<ReplaceFields>& replace,
                               DBCTableType type) {
  (this->*(dbc_method_map_[type]))(dst_path, src_path, replace);
}

void DBCChanger::ChangeSpellDBC(const std::string& dst_path,
                                const std::string& src_path,
                                const std::vector<ReplaceFields>& replace) {
  DBCHandler handler;
  if (handler.Load(src_path) != DBCError::kSuccess) {
    throw std::runtime_error("Failed to load DBC file");
  }

  for (const auto& [record_id, fields] : replace) {
    Record record = handler.GetRecordById(record_id);
    for (const auto& [field, new_value] : fields) {
      record.SetUInt32(field, new_value);
    }
  }

  handler.Save(dst_path);
}
