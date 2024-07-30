/* irrsmo00_conn.h */

#ifndef IRRSMO00_CONN_H_
#define IRRSMO00_CONN_H_

#include <stdbool.h>

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

/* Callable service prototypes */
#pragma linkage(IRRSMO64, OS_NOSTACK)

void null_byte_fix(char *str, unsigned int str_len);

char *call_irrsmo00(char *request_xml, char *running_userid,
                    unsigned int result_buffer_size, int irrsmo00_options,
                    int *saf_rc, int *racf_rc, int *racf_rsn, bool debug);

#endif /* IRRSMO00_CONN_H_ */