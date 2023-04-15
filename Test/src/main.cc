
#include <iostream>

#include "dbc.h"

int main() {
  DBCHandler handler;
  handler.Load("SpellVisualEffectName.dbc");

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
