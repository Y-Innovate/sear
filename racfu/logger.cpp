#include "logger.hpp"

#include <cctype>
#include <csignal>
#include <iomanip>
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

void Logger::hexDump(const char* p_buffer, int length) const {
  if (!debug_) {
    return;
  }

  if (p_buffer == nullptr) {
    std::cout << std::endl << "N/A" << std::endl << std::endl;
    return;
  }

  char p_decoded[length];
  std::memcpy(p_decoded, p_buffer, length);
  __e2a_l(p_decoded, length);

  std::string hex_dump = "\n";
  std::ostringstream hex_stream;
  std::ostringstream decoded_stream;
  for (int i = 0; i < length; i++) {
    if (i % 16 == 0) {
      std::string hex_string = hex_stream.str();
      if (isatty(fileno(stdout))) {
        hex_string.resize(195, ' ');
      } else {
        hex_string.resize(51, ' ');
      }
      if (i != 0) {
        hex_dump += hex_string + decoded_stream.str() + "\n";
      }
      hex_stream.str("");
      hex_stream.clear();
      decoded_stream.str("");
      decoded_stream.clear();
      hex_stream << std::hex << std::setw(8) << std::setfill('0') << i << ":";
    }
    if (i % 2 == 0) {
      hex_stream << " ";
    }
    if (std::isprint(static_cast<unsigned char>(p_decoded[i]))) {
      if (isatty(fileno(stdout))) {
        hex_stream << ansi_bright_green_;
        decoded_stream << ansi_bright_green_ << p_decoded[i] << ansi_reset_;
      } else {
        decoded_stream << p_decoded[i];
      }
    } else {
      if (isatty(fileno(stdout))) {
        if (p_decoded[i] == '\t' or p_decoded[i] == '\r' or
            p_decoded[i] == '\n') {
          hex_stream << ansi_yellow_;
          decoded_stream << ansi_yellow_ << '.' << ansi_reset_;
        } else {
          hex_stream << ansi_red_;
          decoded_stream << ansi_red_ << '.' << ansi_reset_;
        }
      } else {
        decoded_stream << '.';
      }
    }
    hex_stream << std::hex << std::setw(2) << std::setfill('0')
               << (static_cast<int>(p_buffer[i]) & 0xff);
    if (isatty(fileno(stdout))) {
      hex_stream << ansi_reset_;
    }
  }

  std::string hex_string = hex_stream.str();
  if (isatty(fileno(stdout))) {
    hex_string.resize(51 + ((length % 16) * 5) + ((length % 16) * 4), ' ');
  } else {
    hex_string.resize(51, ' ');
  }
  hex_dump += hex_string + decoded_stream.str() + "\n";

  std::cout << hex_dump << std::endl;
}
};  // namespace RACFu
