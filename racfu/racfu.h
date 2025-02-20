#ifndef __RACFU_H_
#define __RACFU_H_

#include "racfu_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
This is the main interface to RACFu.
Note that 'racfu_result_t *result' will be populated with pointers
to buffers that must be freed by direct callers of this interface.

The following pointers must be freed after calling this interface to
avoid memory leaks:

  result.raw_request
  result.raw_result
  result.result_json

'result.raw_request' and 'result.raw_result' contain binary data with
the length of the data for each being provided by
'result.raw_request_length' and 'result.raw_result_length' respectively.
'result.result_json' is an ASCII encoded, null terminated string.
*/
void racfu(racfu_result_t *result, const char *request_json, bool debug);

#ifdef __cplusplus
}
#endif

#pragma export(racfu)

#endif
