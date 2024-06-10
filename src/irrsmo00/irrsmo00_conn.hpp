/* irrsmo00_conn.h */

#ifndef IRRSMO00_CONN_H_
#define IRRSMO00_CONN_H_

#include <stdbool.h>

typedef struct
{
    unsigned char running_userid_length;
    char running_userid[8];
} running_userid_t;

/* Prototype for IRRSMO64 */
extern void IRRSMO64(char*,   // Workarea
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

#pragma export(call_irrsmo00_with_json)

void null_byte_fix(char* str, unsigned int str_len);

char * injson_to_inxml(char * injson, char * userid_buffer, int * irrsmo00_options, unsigned int * result_buffer_size, bool * debug);
char * outxml_to_outjson(char * outxml, int saf_rc, int racf_rc, int racf_rsn, bool debug);

char * call_irrsmo00(
    char * request_xml, char * running_userid, unsigned int result_buffer_size, int irrsmo00_options,
    int * saf_rc, int * racf_rc, int * racf_rsn, bool debug
    );

extern "C" {
    char * call_irrsmo00_with_json(char * json_req_string);
}

#endif /* IRRSMO00_CONN_H_ */