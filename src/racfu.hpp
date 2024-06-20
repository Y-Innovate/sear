#ifndef __RACFU_H_
#define __RACFU_H_

#include "racfu_result.hpp"

extern "C" {
  void racfu(racfu_result_t *result, char *request_json);
}

#pragma export(racfu)

#endif
