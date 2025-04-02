#ifndef __RACFU_LOGGER_H_
#define __RACFU_LOGGER_H_

#include <iostream>
#include <string>

namespace RACFu {
class Logger {
 private:
  bool debug_;
  char max_line_length_    = 80;
  std::string ansi_yellow_ = "\033[33m";
  std::string ansi_reset_  = "\033[0m";
  explicit Logger();

 public:
  explicit Logger(Logger const&) = delete;
  void operator=(Logger const&)  = delete;
  static Logger& getInstance();
  void setDebug(bool debug);
  void debug(const std::string& message, const std::string& body = "") const;
  void debugAllocate(const void* ptr, int rmode, int byte_count) const;
  void debugFree(const void* ptr, int rmode) const;
  std::string castHexString(const char* input, int buffer_len = 0) const;
};
}  // namespace RACFu

#endif
