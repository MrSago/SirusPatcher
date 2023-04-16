#include "dbc/dbccreator.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "dbc/dbchandler.h"

DBCCreator::DBCCreator() {
  dbc_method_map_[DBCTableType::kSpell] = &DBCCreator::CreateSpellDBC;
}

DBCCreator::~DBCCreator() = default;

void DBCCreator::CreateDBCFile(const std::string& dst_path,
                               const std::string& src_path,
                               const std::vector<ReplaceFields>& replace, DBCTableType type) {
  (this->*(dbc_method_map_[type]))(dst_path, src_path, replace);
}

void DBCCreator::CreateSpellDBC(const std::string& dst_path,
                                const std::string& src_path,
                                const std::vector<ReplaceFields>& replace) {
  DBCHandler handler;
  if (handler.Load(src_path) != DBCError::kSuccess) {
    std::cout << "Failed to load DBC file\n";
    return;
  }

  for (const auto& [record_id, fields] : replace) {
    Record record = handler.GetRecordById(record_id);
    for (const auto& [field, new_value] : fields) {
      record.SetUInt32(field, new_value);
    }
  }

  handler.Save(dst_path);
}
