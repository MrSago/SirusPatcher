#include <iostream>
#include <vector>

#include "dbc/dbcchanger.h"

int main() {
  const std::string kSpellDBCPath = "Spell.dbc";
  const std::string kDstSpellDBCPath = "Test.dbc";
  std::vector<ReplaceFields> repl = {
      {10, {{2, 333}, {3, 777}}},
      {12, {{3, 444}, {4, 888}, {5, 0x12345678}}}
  };

  DBCChanger creator;
  creator.ChangeDBCFile(kDstSpellDBCPath, kSpellDBCPath, repl,
                        DBCTableType::kSpell);

  // DBCHandler handler;
  // if (handler.Load("SpellVisualEffectName.dbc") != DBCError::kSuccess) {
  //   std::cout << "Failed to load DBC file\n";
  //   return 1;
  // }

  // Record rec1 = handler.GetRecordById(7673);
  // Record rec2 = handler.GetRecordById(6);

  // std::cout << rec1.GetString(2) << '\n';
  // std::cout << rec2.GetString(3) << '\n';

  // rec2.SetStringRef(3, "test");

  // std::cout << rec1.GetString(2) << '\n';
  // std::cout << rec2.GetString(3) << '\n';

  // handler.Save("test.dbc");

  return 0;
}
