#ifndef __RACFU_ERRORS_H_
#define __RACFU_ERRORS_H_

#include <string>
#include <vector>

namespace RACFu {
class Errors {
 public:
  std::vector<std::string> error_messages = {};
  void add_racfu_error_message(std::string error_message);
  void add_irrsmo00_error_message(std::string error_message);
  bool empty();

 private:
  void add_error_message(std::string error_message);
};
}  // namespace RACFu

#endif
