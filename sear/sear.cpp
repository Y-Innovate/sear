#include "sear.h"

#include "security_admin.hpp"

sear_result_t *sear(const char *request_json, const std::string& encoding, int length, bool debug) {
  static sear_result_t sear_result   = {nullptr, 0, nullptr, 0, nullptr};
  SEAR::SecurityAdmin security_admin = SEAR::SecurityAdmin(&sear_result, debug);
  security_admin.makeRequest(request_json, length, encoding);
  return &sear_result;
}
