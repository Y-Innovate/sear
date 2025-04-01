#include "irrsmo00_error.hpp"

#include <algorithm>

namespace RACFu {
// cppcheck-suppress passedByValue
IRRSMO00Error::IRRSMO00Error(std::vector<std::string> errors)
    : errors_(errors) {
  std::for_each(errors_.begin(), errors_.end(),
                [](std::string& error) { error = "irrsmo00: " + error; });
}

// cppcheck-suppress passedByValue
IRRSMO00Error::IRRSMO00Error(std::string error)
    : errors_({"irrsmo00: " + error}) {}
}  // namespace RACFu
