#ifndef __RACFU_SECURITY_REQUEST_H_
#define __RACFU_SECURITY_REQUEST_H_

#include <nlohmann/json.hpp>

#include "errors.hpp"
#include "logger.hpp"
#include "racfu_result.h"

namespace RACFu {

class SecurityRequest {
 public:
  SecurityRequest();
  explicit SecurityRequest(racfu_result_t* result);
  void load(const nlohmann::json& request);
  void build_result(const nlohmann::json& intermediate_result_json,
                    const Errors& errors, const Logger& logger);
  // Request
  std::string admin_type;
  std::string operation;
  std::string profile_name;
  std::string group;  // Only used for group connection
  std::string volume;
  std::string generic;
  std::string class_name;
  char run_as_userid[8] = {0};
  nlohmann::json traits;
  uint8_t function_code = 0;
  int irrsmo00_options  = 13;
  // Result
  racfu_result_t* result;
  racfu_return_codes_t return_codes = {-1, -1, -1, -1};
};

}  // namespace RACFu

#endif
