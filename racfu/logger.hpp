#ifndef __RACFU_LOGGER_H_
#define __RACFU_LOGGER_H_

#include <iostream>
#include <string>

#define YELLOW_COLOR "\033[33m"
#define NO_COLOR "\033[0m"

#define MAX_LINE_LENGTH 80
#define HEX_CHAR_SIZE 6

// Common Logging Messages
#define MSG_VALIDATING_PARAMETERS "Validating parameters ..."
#define MSG_DONE "Done"
#define MSG_BUILD_RESULT "Building JSON result ..."

// IRRSEQ00 Logging Messages
#define MSG_SEQ_PATH "Entering IRRSEQ00 path"
#define MSG_CALLING_SEQ "Calling IRRSEQ00 ..."
#define MSG_REQUEST_SEQ_GENERIC "Generic extract request buffer:"
#define MSG_REQUEST_SEQ_SETROPTS "Setropts extract request buffer:"
#define MSG_RESULT_SEQ_GENERIC "Raw profile extract result:"
#define MSG_RESULT_SEQ_SETROPTS "Raw setropts extract result:"
#define MSG_SEQ_POST_PROCESS "Profile extract result has been post-processed"

// IRRSMO00 Logging Messages
#define MSG_SMO_PATH "Entering IRRSMO00 path"
#define MSG_RUN_AS_USER "Running under the authority of user: "
#define MSG_VALIDATING_TRAITS "Validating traits ..."
#define MSG_REQUEST_SMO_ASCII "Request XML:"
#define MSG_REQUEST_SMO_EBCDIC "EBCDIC encoded request XML:"
#define MSG_CALLING_SMO "Calling IRRSMO00 ..."
#define MSG_RESULT_SMO_EBCDIC "Raw EBCDIC encoded result XML:"
#define MSG_RESULT_SMO_ASCII "Decoded result XML:"

class Logger {
 private:
  bool debug;

 public:
  Logger(bool debug) : debug(debug) {};
  std::string cast_hex_string(char* input, int buffer_len = 0);
  void log_debug(std::string message, std::string body = "");
};

#endif
