#include "errors.hpp"

namespace RACFu {
void Errors::add_racfu_error_message(const std::string& error_message) {
  this->add_error_message("racfu: " + error_message);
}

void Errors::add_irrsmo00_error_message(const std::string& error_message) {
  this->add_error_message("irrsmo00: " + error_message);
}

void Errors::add_error_message(const std::string& error_message) {
  this->error_messages.push_back(error_message);
}

bool Errors::empty() const {
  if (this->error_messages.size() == 0) {
    return true;
  }
  return false;
}
}  // namespace RACFu
