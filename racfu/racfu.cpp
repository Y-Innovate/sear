#include "racfu.h"

#include <pthread.h>

#include "security_admin.hpp"

pthread_mutex_t racfu_mutex = PTHREAD_MUTEX_INITIALIZER;

racfu_result_t *racfu(const char *request_json, int length, bool debug) {
  static racfu_result_t racfu_result = {nullptr, 0, nullptr, 0, nullptr};
  RACFu::SecurityAdmin security_admin =
      RACFu::SecurityAdmin(&racfu_result, debug);
  security_admin.makeRequest(request_json, length);
  return &racfu_result;
}
