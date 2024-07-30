/* irrsmo64.hpp */

#ifndef _irrsmo64_test
#define _irrsmo64_test

#define REQUEST_XML_BASIC_SIZE 226
#define RESULT_XML_BASIC_SIZE 1252

#define REQUEST_XML_TEST_SIZE 196
#define RESULT_XML_SIZE 2328
#define PARTIAL_XML_SIZE 201

typedef struct {
    unsigned char running_userid_length;
    char running_userid[8];
} running_userid_t;

extern "C" {
void IRRSMO64(char *,        // Workarea
              int *, int *,  // safrc
              int *, int *,  // racfrc
              int *, int *,  // racfrsn
              int *,         // Numparms
              int *,         // Function code
              int *,         // options
              int *,         // Request Length
              char *,        // Request
              char *,        // Request Handle
              char *,        // run as user
              int *,         // ACEE (not used)
              int *,         // Result buffer
              char *         // Result
);
}

#endif