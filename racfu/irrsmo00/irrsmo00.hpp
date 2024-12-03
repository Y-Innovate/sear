#ifndef _IRRSMO00_H_
#define _IRRSMO00_H_

#include <stdbool.h>

#include <nlohmann/json.hpp>
#include <string>

#include "racfu_result.h"

typedef struct {
  unsigned char running_userid_length;
  char running_userid[8];
} running_userid_t;

/* Prototype for IRRSMO64 */
extern "C" {
void IRRSMO64(char *,               // Workarea
              unsigned int, int *,  // safrc
              unsigned int, int *,  // racfrc
              unsigned int, int *,  // racfrsn
              int *,                // Numparms
              int *,                // Function code
              int *,                // options
              int *,                // Request Length
              char *,               // Request
              char *,               // Request Handle
              char *,               // run as user
              unsigned int,         // ACEE (not used)
              int *,                // Result buffer
              char *                // Result
);
}

// We need to ignore this pragma for unit tests since the
// IRRSMO64 mock is compiled for XPLINK.
#ifdef __TOS_390__
#pragma linkage(IRRSMO64, OS_NOSTACK)
#endif

char *call_irrsmo00(char *request_xml, char *running_userid,
                    unsigned int *result_buffer_size_p, int irrsmo00_options,
                    int *saf_rc_p, int *racf_rc_p, int *racf_rsn_p);

bool does_profile_exist(std::string admin_type, std::string profile_name,
                        const char *class_name, char *running_userid);

int post_process_smo_json(nlohmann::json *results_p, std::string profile_name,
                          const char *class_name);

#endif /* IRRSMO00_H_ */
