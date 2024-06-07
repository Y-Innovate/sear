/* irrsmo64.h */

#ifndef _irrsmo64_test
#define _irrsmo64_test

#define REQUEST_XML_BASIC_SIZE 219
#define RESULT_XML_BASIC_SIZE 1252

#define REQUEST_XML_TEST_SIZE 213
#define RESULT_XML_SIZE 2328
#define PARTIAL_XML_SIZE 201


typedef struct
{
    unsigned char running_userid_length;
    char running_userid[8];
} running_userid_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif