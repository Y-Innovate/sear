#ifndef __RACFU_LOGGER_H_
#define __RACFU_LOGGER_H_

#include <iostream>
#include <string>

#define YELLOW_COLOR "\033[33m"
#define NO_COLOR "\033[0m"

#define MAX_LINE_LENGTH 80
#define HEX_CHAR_SIZE 6

class Logger {
 private:
  bool debug_mode;

 public:
  Logger(bool debug) : debug_mode(debug) {};
  std::string cast_hex_string(char* input, int buffer_len = 0);
  void debug(std::string message, std::string body = "");
};

#endif
