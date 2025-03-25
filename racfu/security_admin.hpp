#ifndef __RACFU_SECURITY_ADMIN_H_
#define __RACFU_SECURITY_ADMIN_H_

#include <cstdint>
#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>

#include "errors.hpp"
#include "logger.hpp"
#include "racfu_result.h"
#include "security_request.hpp"

namespace RACFu {
static nlohmann::json parameters_schema = RACFU_PARAMETERS_SCHEMA;
static nlohmann::json_schema::json_validator parameter_validator{
    parameters_schema};

class SecurityAdmin {
 public:
  SecurityAdmin(racfu_result_t *result, bool debug);
  void make_request(const char *request_json_string);

 private:
  SecurityRequest request;
  Logger logger;
  Errors errors;
  void do_extract();
  void do_add_alter_delete();
};
}  // namespace RACFu

#endif
