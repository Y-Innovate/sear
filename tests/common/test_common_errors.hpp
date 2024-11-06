#ifndef __TEST_COMMON_ERRORS_H_
#define __TEST_COMMON_ERRORS_H_

#define COMMON_REQUEST_SAMPLES "./tests/common/request_samples/"
#define COMMON_RESULT_SAMPLES "./tests/common/result_samples/"

// Request samples
#define TEST_PARMS_JUNK_REQUEST_JSON \
  COMMON_REQUEST_SAMPLES "test_parameters_junk.json"
#define TEST_PARMS_NONSTRING_REQUEST_JSON \
  COMMON_REQUEST_SAMPLES "test_parameters_nonstring.json"
#define TEST_PARMS_MISSING_REQUEST_JSON \
  COMMON_REQUEST_SAMPLES "test_parameters_missing.json"

// Result samples
#define TEST_PARMS_JUNK_RESULT_JSON \
  COMMON_RESULT_SAMPLES "test_parameters_junk.json"
#define TEST_PARMS_NONSTRING_RESULT_JSON \
  COMMON_RESULT_SAMPLES "test_parameters_nonstring.json"
#define TEST_PARMS_MISSING_RESULT_JSON \
  COMMON_RESULT_SAMPLES "test_parameters_missing.json"

void test_parse_parameters_junk_error(void);
void test_parse_parameters_missing_error(void);
void test_parse_parameters_nonstring_error(void);

#endif