#include "logger.hpp"

#include <csignal>
#include <iostream>
#include <sstream>

#ifdef __TOS_390__
#include <unistd.h>
#else
#include "zoslib.h"
#endif

namespace RACFu {
Logger::Logger() { debug_ = false; }

Logger& Logger::getInstance() {
  static Logger instance;
  return instance;
}

void Logger::setDebug(bool debug) { debug_ = debug; }

void Logger::debug(const std::string& message, const std::string& body) const {
  if (!debug_) {
    return;
  }
  std::string racfu_header = "racfu:";
  if (isatty(fileno(stdout))) {
    racfu_header = ansi_yellow_ + racfu_header + ansi_reset_;
  }
  std::cout << racfu_header << " " << message << "\n";
  if (body != "") {
    for (size_t i = 0; i < body.length(); i += max_line_length_) {
      std::cout << body.substr(i, max_line_length_) << "\n";
    }
  }
}

void Logger::debugAllocate(const void* ptr, int rmode, int byte_count) const {
  if (!debug_) {
    return;
  }
  std::ostringstream oss;
  oss << "Allocated " << byte_count << " bytes in " << rmode
      << "-bit memory at address " << ptr;
  Logger::debug(oss.str());
}

void Logger::debugFree(const void* ptr, int rmode) const {
  if (!debug_) {
    return;
  }
  std::ostringstream oss;
  oss << "Freeing " << rmode << "-bit memory at address " << ptr << " ...";
  Logger::debug(oss.str());
}

std::string Logger::castHexString(const char* input, int buffer_len) const {
  // Cast data to hex so that small strings and buffers of hex values can be
  // printed to represent EBCDIC data
  char hex_char_size = 6;

  std::string output = "";
  char buff[hex_char_size - 1];
  int running_pad_len = 0;

  if (input == nullptr) {
    return "NULL";
  }

  if (buffer_len == 0) {
    buffer_len = strlen(input);
  }

  for (int i = 0; i < buffer_len; i++) {
    std::snprintf(buff, hex_char_size - 1, "0x%02x",
                  (unsigned char)*(input + i));
    output += buff;
    if (i < (buffer_len - 1)) {
      output += ", ";
      if (((i + 2) * hex_char_size + running_pad_len) % max_line_length_ <
          ((i + 1) * hex_char_size + running_pad_len) % max_line_length_) {
        size_t pad =
            max_line_length_ -
            ((i + 1) * hex_char_size + running_pad_len) % max_line_length_;
        output += std::string(pad, ' ');
        running_pad_len += pad;
      }
    }
  }

  return output;
}
};  // namespace RACFu
