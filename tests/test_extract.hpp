#ifndef __TEST_EXTRACT_H_
#define __TEST_EXTRACT_H_

#ifdef __TOS_390__
// arg pointers will be 32-bit when we run on z/OS.
#define TEST_GENERIC_REQUEST_BUFFER_SIZE 1418
#else
// arg pointers will be 64 bit when we run off platform.
#define TEST_GENERIC_REQUEST_BUFFER_SIZE 1474
#endif

// arg area should be the same on every platform
#define TEST_GENERIC_ARG_AREA_SIZE 1362

void check_arg_pointers(char *raw_request);

void test_generate_extract_user_request(void);
void test_parse_extract_user_result(void);
void test_parse_extract_user_result_user_not_found(void);

#endif