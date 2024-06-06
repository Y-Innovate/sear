#ifndef IRRSMO00_CONN_H_
#define IRRSMO00_CONN_H_

#include <stdbool.h>

#ifdef __MVS__
typedef struct
{
    unsigned char running_userid_length;
    char running_userid[8];
} running_userid_t;

/* Prototype for IRRSMO64 */
void IRRSMO64(char*,   // Workarea
    int*, int*, // safrc
    int*, int*, // racfrc
    int*, int*, // racfrsn
    int*,  // Numparms
    int*,  // Function code
    int*,  // options
    int*,  // Request Length
    char *, // Request
    char *, // Request Handle
    char *, // run as user
    int *,  // ACEE (not used)
    int *,  // Result buffer
    char * // Result
    );

/* Callable service prototypes */
#pragma linkage(IRRSMO64,OS_NOSTACK)
#else
#include "irrsmo64.h"
#endif /* __MVS__*/

#pragma export(call_irrsmo00_with_json)

void null_byte_fix(char* str, unsigned int str_len);


extern char * call_irrsmo00(
    char * request_xml, char * running_userid, unsigned int result_buffer_size, int irrsmo00_options,
    int * saf_rc, int * racf_rc, int * racf_rsn, bool debug
    );

char * call_irrsmo00_with_json(char * json_req_string);

#endif /* IRRSMO00_CONN_H_ */