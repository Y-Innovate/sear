#ifndef __TEST_PARAMETER_VALIDATION_H_
#define __TEST_PARAMETER_VALIDATION_H_

#define VALIDATION_REQUEST_SAMPLES "./tests/validation/request_samples/"
#define VALIDATION_RESULT_SAMPLES "./tests/validation/result_samples/"

// Request samples
#define TEST_PARMS_JUNK_REQUEST_JSON \
  VALIDATION_REQUEST_SAMPLES "test_parameters_junk_request.json"
#define TEST_PARMS_NONSTRING_REQUEST_JSON \
  VALIDATION_REQUEST_SAMPLES "test_parameters_nonstring_request.json"
#define TEST_PARMS_MISSING_REQUEST_JSON \
  VALIDATION_REQUEST_SAMPLES "test_parameters_missing_request.json"

// Result samples
#define TEST_PARMS_JUNK_RESULT_JSON \
  VALIDATION_RESULT_SAMPLES "test_parameters_junk_result.json"
#define TEST_PARMS_NONSTRING_RESULT_JSON \
  VALIDATION_RESULT_SAMPLES "test_parameters_nonstring_result.json"
#define TEST_PARMS_MISSING_RESULT_JSON \
  VALIDATION_RESULT_SAMPLES "test_parameters_missing_result.json"

void test_parse_parameters_junk_error();
void test_parse_parameters_missing_error();
void test_parse_parameters_nonstring_error();

#endif
