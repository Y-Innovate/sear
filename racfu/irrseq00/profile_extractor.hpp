#ifndef __RACFU_PROFILE_EXTRACTOR_H_
#define __RACFU_PROFILE_EXTRACTOR_H_

#include <cstdint>
#include <string>

#include "irrseq00.hpp"
#include "logger.hpp"
#include "messages.h"
#include "racfu_result.h"
#include "security_request.hpp"

namespace RACFu {
class ProfileExtractor {
 public:
  explicit ProfileExtractor() {};
  void extract(SecurityRequest &request, const Logger &logger);

 private:
  static void build_generic_extract_request(
      generic_extract_underbar_arg_area_t *arg_area,
      const std::string &profile_name, const std::string &class_name,
      uint8_t function_code);
  static void build_racf_options_extract_request(
      setropts_extract_underbar_arg_area_t *arg_area);
  static void preserve_raw_request(const char *arg_area, char **raw_request,
                                   const int &raw_request_length);
};
}  // namespace RACFu

#endif
