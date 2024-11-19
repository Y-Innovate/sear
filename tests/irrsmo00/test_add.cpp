#include "tests/irrsmo00/test_add.hpp"

#include <sys/stat.h>

#include <cstring>

#include "racfu/racfu.h"
#include "tests/mock/irrsmo64.hpp"
#include "tests/unit_test_utilities.hpp"
#include "tests/unity/unity.h"

void test_generate_add_user_request() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ADD_USER_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_ADD_USER_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ADD_USER_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock = 0;
  irrsmo64_racf_rc_mock = 0;
  irrsmo64_racf_reason_mock = 0;

  racfu(&result, request_json.c_str(), false);

  TEST_ASSERT_EQUAL_INT32(raw_request_size_expected.st_size,
                          result.raw_request_length);
  TEST_ASSERT_EQUAL_MEMORY(raw_request_expected, result.raw_request,
                           raw_request_size_expected.st_size);

  // Cleanup
  free(raw_request_expected);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}

void test_parse_add_user_result() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ADD_USER_REQUEST_JSON);
  std::string result_json_expected = get_json_sample(TEST_ADD_USER_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = get_raw_sample(TEST_ADD_USER_RESULT_RAW);
  struct stat st;
  stat(TEST_ADD_USER_RESULT_RAW, &st);
  irrsmo64_result_size_mock = st.st_size;
  irrsmo64_saf_rc_mock = 0;
  irrsmo64_racf_rc_mock = 0;
  irrsmo64_racf_reason_mock = 0;

  racfu(&result, request_json.c_str(), false);

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);
  TEST_ASSERT_EQUAL_INT32(result_json_expected.length(),
                          strlen(result.result_json));

  // Cleanup
  free(irrsmo64_result_mock);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}

//
void test_parse_add_user_result_user_already_exists() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ADD_USER_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ADD_USER_RESULT_USER_ALREADY_EXISTS_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock =
      get_raw_sample(TEST_ADD_USER_RESULT_USER_ALREADY_EXISTS_RAW);
  struct stat st;
  stat(TEST_ADD_USER_RESULT_USER_ALREADY_EXISTS_RAW, &st);
  irrsmo64_result_size_mock = st.st_size;
  irrsmo64_saf_rc_mock = 4;
  irrsmo64_racf_rc_mock = 4;
  irrsmo64_racf_reason_mock = 0;

  racfu(&result, request_json.c_str(), false);

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);
  TEST_ASSERT_EQUAL_INT32(result_json_expected.length(),
                          strlen(result.result_json));

  // Cleanup
  free(irrsmo64_result_mock);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}

void test_parse_add_user_parameter_errors() {
  racfu_result_t result;
  std::string request_json =
      get_json_sample(TEST_ADD_USER_PARAMETER_ERRORS_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ADD_USER_PARAMETER_ERRORS_RESULT_JSON);

  racfu(&result, request_json.c_str(), false);

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}

void test_parse_add_user_trait_errors() {
  racfu_result_t result;
  std::string request_json =
      get_json_sample(TEST_ADD_USER_TRAIT_ERRORS_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ADD_USER_TRAIT_ERRORS_RESULT_JSON);

  racfu(&result, request_json.c_str(), false);

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}

void test_parse_add_user_no_xml_data_error() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ADD_USER_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ADD_USER_NO_RESPONSE_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = NULL;
  // struct stat st;
  // stat(TEST_ADD_USER_RESULT_USER_ALREADY_EXISTS_RAW, &st);
  // irrsmo64_result_size_mock = st.st_size;
  irrsmo64_saf_rc_mock = 8;
  irrsmo64_racf_rc_mock = 200;
  irrsmo64_racf_reason_mock = 16;

  racfu(&result, request_json.c_str(), false);

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);

  // Cleanup
  free(irrsmo64_result_mock);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}

void test_parse_alter_user_no_xml_data_error() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ALTER_USER_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ALTER_USER_NO_RESPONSE_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = NULL;
  // struct stat st;
  // stat(TEST_ADD_USER_RESULT_USER_ALREADY_EXISTS_RAW, &st);
  // irrsmo64_result_size_mock = st.st_size;
  irrsmo64_saf_rc_mock = 4;
  irrsmo64_racf_rc_mock = 4;
  irrsmo64_racf_reason_mock = 0;

  racfu(&result, request_json.c_str(), false);

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);

  // Cleanup
  free(irrsmo64_result_mock);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}

void test_parse_alter_user_traits_not_json_error() {
  racfu_result_t result;
  std::string request_json =
      get_json_sample(TEST_ALTER_USER_TRAITS_NOT_JSON_ERROR_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ALTER_USER_TRAITS_NOT_JSON_ERROR_RESULT_JSON);

  racfu(&result, request_json.c_str(), false);

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}
