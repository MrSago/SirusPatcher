#ifndef _LADYSPIRITSCHANGER_H_
#define _LADYSPIRITSCHANGER_H_

#include <string>

class LadySpiritsChanger {
 private:
  static const std::string kDbcPath;
  static const std::string kDbcPathSave;

 public:
  void Change();
};

#endif  // _LADYSPIRITSCHANGER_H_
