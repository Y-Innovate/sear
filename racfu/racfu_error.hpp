#ifndef __RACFU_ERROR_H_
#define __RACFU_ERROR_H_

#include <exception>
#include <vector>

namespace RACFu {
class RACFuError : public std::exception {
 public:
  std::vector<std::string> errors_;
  explicit RACFuError(std::vector<std::string> &errors);
  explicit RACFuError(std::string error);
};
}  // namespace RACFu

#endif
