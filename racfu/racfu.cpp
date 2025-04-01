#include "racfu.h"

#include "security_admin.hpp"

void racfu(racfu_result_t *result, const char *request_json, bool debug) {
  RACFu::SecurityAdmin security_admin = RACFu::SecurityAdmin(result, debug);
  security_admin.makeRequest(request_json);
}
