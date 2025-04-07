#ifndef __RACFU_SECURITY_REQUEST_H_
#define __RACFU_SECURITY_REQUEST_H_

#include <nlohmann/json.hpp>
#include <vector>

#include "logger.hpp"
#include "racfu_result.h"

namespace RACFu {

class SecurityRequest {
 public:
  // Request
  std::string admin_type_;
  std::string operation_;
  std::string profile_name_;
  std::string group_;  // Only used by IRRSMO00 for group connection
  std::string volume_;
  std::string generic_;
  std::string class_name_;
  char run_as_userid_[8] = {0};
  nlohmann::json traits_;
  uint8_t function_code_ = 0;
  int irrsmo00_options_  = 13;
  // Result
  racfu_result_t* p_result_;
  racfu_return_codes_t return_codes_ = {-1, -1, -1, -1};
  std::vector<std::string> errors_;
  nlohmann::json intermediate_result_json_;
  SecurityRequest();
  explicit SecurityRequest(racfu_result_t* p_result);
  void load(const nlohmann::json& request);
  void buildResult();
};

}  // namespace RACFu

#endif
