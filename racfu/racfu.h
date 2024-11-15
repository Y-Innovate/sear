#ifndef __RACFU_H_
#define __RACFU_H_

#include "racfu_result.h"

#ifdef __cplusplus
extern "C" {
#endif

void racfu(racfu_result_t *result, const char *request_json,
           bool debug = false);

#ifdef __cplusplus
}
#endif

#pragma export(racfu)

#endif
