#include "tests/irrsmo00/test_common_errors.hpp"

#include <string.h>
#include <sys/stat.h>

#include "racfu/racfu.h"
#include "racfu/racfu_result.h"
#include "tests/unit_test_utilities.hpp"
#include "tests/unity/unity.h"

void test_parse_parameters_junk_error(void) {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_PARMS_JUNK_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_PARMS_JUNK_RESULT_JSON);

  int expected_racfu_rc = 8;

  racfu(&result, request_json.c_str());

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);
  TEST_ASSERT_EQUAL_INT32(expected_racfu_rc,
                          result.result_json.return_codes->racfu_return_code);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}

void test_parse_parameters_missing_error(void) {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_PARMS_MISSING_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_PARMS_MISSING_RESULT_JSON);

  int expected_racfu_rc = 8;

  racfu(&result, request_json.c_str());

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);
  TEST_ASSERT_EQUAL_INT32(expected_racfu_rc,
                          result.result_json.return_codes->racfu_return_code);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}

void test_parse_parameters_nonstring_error(void) {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_PARMS_NONSTRING_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_PARMS_NONSTRING_RESULT_JSON);

  int expected_racfu_rc = 8;

  racfu(&result, request_json.c_str());

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);
  TEST_ASSERT_EQUAL_INT32(expected_racfu_rc,
                          result.result_json.return_codes->racfu_return_code);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}