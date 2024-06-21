#ifndef __RACFU_RESULT_H_
#define __RACFU_RESULT_H_

typedef struct {
    char *raw_result;
    int raw_result_length;
    char *result_json;
} racfu_result_t;

typedef struct {
    int saf_return_code;
    int racf_return_code;
    int racf_reason_code;
    int irrseq00_return_code;
    int irrsmo00_return_code;
    int racfu_return_code;
} racfu_return_codes_t;

#endif