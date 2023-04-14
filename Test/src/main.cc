
#include <iostream>

#include "dbc.h"

int main() {
  DBCHandler handler;
  handler.Load("Spell.dbc");

  Record record1 = handler[3];
  std::cout << record1.GetString(139) << '\n';
  record1.SetStringRef(139, "Hello, world!");
  std::cout << record1.GetString(139) << '\n';

  handler.Save("test.dbc");

  handler.Load("test.dbc");
  Record record2 = handler[3];
  std::cout << "\n\n" << record2.GetString(139) << '\n';

  return 0;
}
