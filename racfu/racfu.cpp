#include "racfu.h"

#include "security_admin.hpp"

racfu_result_t *racfu(const char *request_json, bool debug) {
  static racfu_result_t racfu_result = {NULL, 0, NULL, 0, NULL};
  RACFu::SecurityAdmin security_admin =
      RACFu::SecurityAdmin(&racfu_result, debug);
  security_admin.makeRequest(request_json);
  return &racfu_result;
}
