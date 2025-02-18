#ifndef __RACFU_SECURITY_ADMIN_H_
#define __RACFU_SECURITY_ADMIN_H_

#include <nlohmann/json.hpp>

#include "errors.hpp"
#include "logger.hpp"
#include "racfu_result.h"

namespace RACFu {
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
  void do_extract();
  void do_add_alter_delete();
  void build_result(char *raw_request, int raw_request_length, char *raw_result,
                    int raw_result_length,
                    const nlohmann::json &intermediate_result_json);
};
}  // namespace RACFu

#endif
