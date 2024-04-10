#ifndef IRRSMO00_CONN_H_
#define IRRSMO00_CONN_H_

#include <stdbool.h>

#ifdef __MVS__
#pragma linkage(IRRSMO64, OS)
typedef struct
{
    unsigned char running_userid_length;
    char running_userid[8];
} running_userid_t;
#else
#include "irrsmo64.h"
#endif /* __MVS__*/

#pragma export(call_irrsmo00_with_json)

void null_byte_fix(char* str, unsigned int str_len);

char * call_irrsmo00(
    char * request_xml, char * running_userid, unsigned int result_buffer_size, unsigned int irrsmo00_options,
    unsigned int * saf_rc, unsigned int * racf_rc, unsigned int * racf_rsn, bool debug
    );

char * call_irrsmo00_with_json(
    char * json_req_string,
    unsigned int * saf_rc, unsigned int * racf_rc, unsigned int * racf_rsn
);

#endif /* IRRSMO00_CONN_H_ */