#ifndef __TEST_ADD_H_
#define __TEST_ADD_H_

#define IRRSMO00_REQUEST_SAMPLES "./tests/irrsmo00/request_samples/"
#define IRRSMO00_RESULT_SAMPLES "./tests/irrsmo00/result_samples/"

// Request samples
#define TEST_ADD_USER_REQUEST_JSON \
  IRRSMO00_REQUEST_SAMPLES "test_add_user_request.json"
#define TEST_ADD_USER_REQUEST_RAW \
  IRRSMO00_REQUEST_SAMPLES "test_add_user_request.bin"

// Result samples
#define TEST_ADD_USER_RESULT_JSON \
  IRRSMO00_RESULT_SAMPLES "test_add_user_result.json"
#define TEST_ADD_USER_RESULT_RAW \
  IRRSMO00_RESULT_SAMPLES "test_add_user_result.bin"
#define TEST_ADD_USER_RESULT_USER_ALREADY_EXISTS_JSON \
  IRRSMO00_RESULT_SAMPLES                             \
  "test_add_user_result_user_already_exists.json"
#define TEST_ADD_USER_RESULT_USER_ALREADY_EXISTS_RAW \
  IRRSMO00_RESULT_SAMPLES                            \
  "test_add_user_result_user_already_exists.bin"

void test_generate_add_user_request(void);
void test_parse_add_user_result(void);
void test_parse_add_user_result_user_already_exists(void);

#endif