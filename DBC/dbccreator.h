#ifndef _DBC_CREATOR_H
#define _DBC_CREATOR_H

#include <cstdint>
#include <map>
#include <vector>

#include "dbc/dbchandler.h"

enum class DBCTableType { kSpell = 0, kSpellVisualEffectName, kSpellVisualKit };

struct Field {
  uint32_t field;
  uint32_t new_value;
};

struct ReplaceFields {
  uint32_t record_id;
  std::vector<Field> fields;
};

class DBCCreator {
 public:
  DBCCreator();
  ~DBCCreator();

  void CreateDBCFile(const std::string& dst_path,
                     const std::string& src_path,
                     const std::vector<ReplaceFields>& replace,
                     DBCTableType type);

 private:
  void CreateSpellDBC(const std::string& dst_path,
                      const std::string& src_path,
                      const std::vector<ReplaceFields>& replace);

  // void CreateSpellVisualEffectNameDBC(const std::string& filename_dst,
  //                                     const std::string& filename_src,
  //                                     const json_map_t& map);
  // void CreateSpellVisualKitDBC(const std::string& filename_dst,
  //                              const std::string& filename_src,
  //                              const json_map_t& map);

  using CreateMethodSign =
      void (DBCCreator::*)(const std::string&, const std::string&,
                           const std::vector<ReplaceFields>&);

  std::map<DBCTableType, CreateMethodSign> dbc_method_map_;
};

#endif  // _DBC_CREATOR_H
