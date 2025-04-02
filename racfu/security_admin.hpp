#ifndef __RACFU_SECURITY_ADMIN_H_
#define __RACFU_SECURITY_ADMIN_H_

#include <cstdint>
#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>

#include "logger.hpp"
#include "racfu_result.h"
#include "security_request.hpp"

namespace RACFu {
static nlohmann::json parameters_schema = RACFU_PARAMETERS_SCHEMA;
static nlohmann::json_schema::json_validator parameter_validator{
    parameters_schema};

class SecurityAdmin {
 private:
  SecurityRequest request_;
  void doExtract();
  void doAddAlterDelete();

 public:
  SecurityAdmin(racfu_result_t *p_result, bool debug);
  void makeRequest(const char *p_request_json_string);
};
}  // namespace RACFu

#endif
