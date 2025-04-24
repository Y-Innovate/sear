#include "racfu.h"

extern "C" int LLVMFuzzerTestOneInput(const char *request_json, int length) {
  racfu(request_json, length, false);
  return 0;
}
