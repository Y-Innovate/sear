#include "racfu_error.hpp"

#include <algorithm>

namespace RACFu {
RACFuError::RACFuError(std::vector<std::string>& errors) : errors_(errors) {
  std::for_each(errors_.begin(), errors_.end(),
                [](std::string& error) { error = "racfu: " + error; });
}

// cppcheck-suppress passedByValue
RACFuError::RACFuError(std::string error) : errors_({"racfu: " + error}) {}
}  // namespace RACFu
