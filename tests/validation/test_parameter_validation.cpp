#include "tests/validation/test_parameter_validation.hpp"

#include <sys/stat.h>

#include <cstring>

#include "racfu/racfu.h"
#include "racfu/racfu_result.h"
#include "tests/unit_test_utilities.hpp"
#include "tests/unity/unity.h"

void test_parse_parameters_junk_error() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_PARMS_JUNK_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_PARMS_JUNK_RESULT_JSON);

  racfu(&result, request_json.c_str());

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}

void test_parse_parameters_missing_error() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_PARMS_MISSING_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_PARMS_MISSING_RESULT_JSON);

  racfu(&result, request_json.c_str());

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}

void test_parse_parameters_nonstring_error() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_PARMS_NONSTRING_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_PARMS_NONSTRING_RESULT_JSON);

  racfu(&result, request_json.c_str());

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}
