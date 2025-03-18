#ifndef __RACFU_SECURITY_ADMIN_H_
#define __RACFU_SECURITY_ADMIN_H_

#include <cstdint>
#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>

#include "errors.hpp"
#include "logger.hpp"
#include "racfu_result.h"

namespace RACFu {

static nlohmann::json parameters_schema = RACFU_PARAMETERS_SCHEMA;
static nlohmann::json_schema::json_validator parameter_validator{
    parameters_schema};

class SecurityAdmin {
 public:
  SecurityAdmin(racfu_result_t *result, bool debug);
  void make_request(const char *request_json);

 private:
  racfu_result_t *result;
  Logger logger;
  Errors errors;
  nlohmann::json request;
  racfu_return_codes_t return_codes;
  std::string admin_type;
  std::string operation;
  std::string profile_name;
  std::string class_name;
  std::string auth_id;
  uint8_t function_code = 0;
  void prepare();
  void do_extract();
  void do_add_alter_delete();
  void build_result(char *raw_request, int raw_request_length, char *raw_result,
                    int raw_result_length,
                    const nlohmann::json &intermediate_result_json);
};
}  // namespace RACFu

#endif
