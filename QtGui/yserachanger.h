#ifndef _YSERACHANGER_H_
#define _YSERACHANGER_H_

#include <string>

class YseraChanger {
 private:
  static const std::string kDbcPath;
  static const std::string kDbcPathSave;

 public:
  void Change();
};

#endif  // _YSERACHANGER_H_
