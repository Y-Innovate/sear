#ifndef __IRRSMO00_H_
#define __IRRSMO00_H_

#include <stdbool.h>

#include <nlohmann/json.hpp>
#include <string>

#include "errors.hpp"
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

char *call_irrsmo00(char *request_xml, const char *running_userid,
                    unsigned int &result_buffer_size, int irrsmo00_options,
                    racfu_return_codes_t &return_codes);

bool does_profile_exist(const std::string &admin_type,
                        const std::string &profile_name,
                        const std::string &class_name,
                        const char *running_userid);

int post_process_smo_json(RACFu::Errors &errors, nlohmann::json &results,
                          const std::string &profile_name,
                          const std::string &admin_type,
                          const std::string &class_name);

#endif /* IRRSMO00_H_ */
