#include <iostream>

#include "DBC/dbchandler.h"

int main() {
  DBCHandler handler;
  if (handler.Load("SpellVisualEffectName.dbc") != DBCError::kSuccess) {
    std::cout << "Failed to load DBC file\n";
    return 1;
  }

  Record rec1 = handler.GetRecordById(7673);
  Record rec2 = handler.GetRecordById(7674);

  std::cout << rec1.GetString(2) << '\n';
  std::cout << rec2.GetString(2) << '\n';

  rec1.SetStringRef(2, "test");

  std::cout << rec1.GetString(2) << '\n';
  std::cout << rec2.GetString(2) << '\n';

  handler.Save("test.dbc");

  return 0;
}
