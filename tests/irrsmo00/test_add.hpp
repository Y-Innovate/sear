#ifndef __TEST_ADD_H_
#define __TEST_ADD_H_

// Request samples
#define TEST_ADD_USER_REQUEST_JSON \
  "./tests/irrsmo00/request_samples/test_add_user_request.json"
#define TEST_ADD_USER_REQUEST_RAW \
  "./tests/irrsmo00/request_samples/test_add_user_request.bin"

// Result samples
#define TEST_ADD_USER_RESULT_JSON \
  "./tests/irrsmo00/result_samples/test_add_user_result.json"
#define TEST_ADD_USER_RESULT_RAW \
  "./tests/irrsmo00/result_samples/test_add_user_result.bin"
#define TEST_ADD_USER_RESULT_USER_ALREADY_EXISTS_JSON \
  "./tests/irrsmo00/result_samples/"                  \
  "test_add_user_result_user_already_exists.json"
#define TEST_ADD_USER_RESULT_USER_ALREADY_EXISTS_RAW \
  "./tests/irrsmo00/result_samples/"                 \
  "test_add_user_result_user_already_exists.bin"

void test_generate_add_user_request(void);
void test_parse_add_user_result(void);
void test_parse_add_user_result_user_already_exists(void);

#endif