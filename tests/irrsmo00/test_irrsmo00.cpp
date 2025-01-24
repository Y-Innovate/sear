#include "tests/irrsmo00/test_irrsmo00.hpp"

#include <sys/stat.h>

#include <cstring>

#include "racfu/racfu.h"
#include "tests/mock/irrsmo64.hpp"
#include "tests/unit_test_utilities.hpp"
#include "tests/unity/unity.h"

// User tests
void test_generate_add_user_request() {
  racfu_result_t result;
  std::string request_json   = get_json_sample(TEST_ADD_USER_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_ADD_USER_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ADD_USER_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_generate_alter_user_request() {
  racfu_result_t result;
  std::string request_json   = get_json_sample(TEST_ALTER_USER_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_ALTER_USER_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ALTER_USER_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_generate_delete_user_request() {
  racfu_result_t result;
  std::string request_json   = get_json_sample(TEST_DELETE_USER_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_DELETE_USER_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_DELETE_USER_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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
  struct stat raw_request_size_expected;
  stat(TEST_ADD_USER_RESULT_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_delete_user_result() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_DELETE_USER_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_DELETE_USER_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = get_raw_sample(TEST_DELETE_USER_RESULT_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_DELETE_USER_RESULT_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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
  struct stat raw_request_size_expected;
  stat(TEST_ADD_USER_RESULT_USER_ALREADY_EXISTS_RAW,
       &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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
  test_validation_errors(TEST_ADD_USER_PARAMETER_ERRORS_REQUEST_JSON,
                         TEST_ADD_USER_PARAMETER_ERRORS_RESULT_JSON);
}

void test_parse_add_user_trait_errors() {
  test_validation_errors(TEST_ADD_USER_TRAIT_ERRORS_REQUEST_JSON,
                         TEST_ADD_USER_TRAIT_ERRORS_RESULT_JSON);
}

void test_parse_add_user_no_xml_data_error() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ADD_USER_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ADD_USER_NO_RESPONSE_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_saf_rc_mock      = 8;
  irrsmo64_racf_rc_mock     = 200;
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
  irrsmo64_result_mock      = NULL;
  irrsmo64_saf_rc_mock      = 4;
  irrsmo64_racf_rc_mock     = 4;
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
  test_validation_errors(TEST_ALTER_USER_TRAITS_NOT_JSON_ERROR_REQUEST_JSON,
                         TEST_ALTER_USER_TRAITS_NOT_JSON_ERROR_RESULT_JSON);
}

void test_parse_irrsmo00_errors_result() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ADD_USER_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_IRRSMO00_ERROR_STRUCTURE_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = get_raw_sample(TEST_IRRSMO00_ERROR_STRUCTURE_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_IRRSMO00_ERROR_STRUCTURE_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 8;
  irrsmo64_racf_rc_mock     = 2000;
  irrsmo64_racf_reason_mock = 68;

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

void test_parse_delete_user_trait_error_result() {
  test_validation_errors(TEST_DELETE_USER_WITH_TRAITS_REQUEST_JSON,
                         TEST_DELETE_USER_WITH_TRAITS_RESULT_JSON);
}

// Group tests
void test_generate_add_group_request() {
  racfu_result_t result;
  std::string request_json   = get_json_sample(TEST_ADD_GROUP_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_ADD_GROUP_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ADD_GROUP_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_generate_alter_group_request() {
  racfu_result_t result;
  std::string request_json   = get_json_sample(TEST_ALTER_GROUP_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_ALTER_GROUP_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ALTER_GROUP_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_generate_delete_group_request() {
  racfu_result_t result;
  std::string request_json   = get_json_sample(TEST_DELETE_GROUP_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_DELETE_GROUP_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_DELETE_GROUP_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_add_group_result() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ADD_GROUP_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ADD_GROUP_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = get_raw_sample(TEST_ADD_GROUP_RESULT_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ADD_GROUP_RESULT_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_delete_group_result() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_DELETE_GROUP_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_DELETE_GROUP_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = get_raw_sample(TEST_DELETE_GROUP_RESULT_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_DELETE_GROUP_RESULT_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_add_group_result_group_already_exists() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ADD_GROUP_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ADD_GROUP_RESULT_GROUP_ALREADY_EXISTS_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock =
      get_raw_sample(TEST_ADD_GROUP_RESULT_GROUP_ALREADY_EXISTS_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ADD_GROUP_RESULT_GROUP_ALREADY_EXISTS_RAW,
       &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_add_group_parameter_errors() {
  test_validation_errors(TEST_ADD_GROUP_PARAMETER_ERRORS_REQUEST_JSON,
                         TEST_ADD_GROUP_PARAMETER_ERRORS_RESULT_JSON);
}

void test_parse_add_group_trait_errors() {
  test_validation_errors(TEST_ADD_GROUP_TRAIT_ERRORS_REQUEST_JSON,
                         TEST_ADD_GROUP_TRAIT_ERRORS_RESULT_JSON);
}

// Group-Connection tests
void test_generate_alter_group_connection_request() {
  racfu_result_t result;
  std::string request_json =
      get_json_sample(TEST_ALTER_GROUP_CONNECTION_REQUEST_JSON);
  char *raw_request_expected =
      get_raw_sample(TEST_ALTER_GROUP_CONNECTION_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ALTER_GROUP_CONNECTION_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_generate_delete_group_connection_request() {
  racfu_result_t result;
  std::string request_json =
      get_json_sample(TEST_DELETE_GROUP_CONNECTION_REQUEST_JSON);
  char *raw_request_expected =
      get_raw_sample(TEST_DELETE_GROUP_CONNECTION_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_DELETE_GROUP_CONNECTION_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_alter_group_connection_result() {
  racfu_result_t result;
  std::string request_json =
      get_json_sample(TEST_ALTER_GROUP_CONNECTION_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ALTER_GROUP_CONNECTION_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = get_raw_sample(TEST_ALTER_GROUP_CONNECTION_RESULT_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ALTER_GROUP_CONNECTION_RESULT_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_delete_group_connection_result() {
  racfu_result_t result;
  std::string request_json =
      get_json_sample(TEST_DELETE_GROUP_CONNECTION_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_DELETE_GROUP_CONNECTION_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock =
      get_raw_sample(TEST_DELETE_GROUP_CONNECTION_RESULT_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_DELETE_GROUP_CONNECTION_RESULT_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_alter_group_connection_parameter_errors() {
  test_validation_errors(
      TEST_ALTER_GROUP_CONNECTION_PARAMETER_ERRORS_REQUEST_JSON,
      TEST_ALTER_GROUP_CONNECTION_PARAMETER_ERRORS_RESULT_JSON);
}

void test_parse_alter_group_connection_trait_errors() {
  test_validation_errors(TEST_ALTER_GROUP_CONNECTION_TRAIT_ERRORS_REQUEST_JSON,
                         TEST_ALTER_GROUP_CONNECTION_TRAIT_ERRORS_RESULT_JSON);
}

// Racf-Options tests
void test_generate_alter_racf_options_request() {
  racfu_result_t result;
  std::string request_json =
      get_json_sample(TEST_ALTER_RACF_OPTIONS_REQUEST_JSON);
  char *raw_request_expected =
      get_raw_sample(TEST_ALTER_RACF_OPTIONS_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ALTER_RACF_OPTIONS_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_alter_racf_options_result() {
  racfu_result_t result;
  std::string request_json =
      get_json_sample(TEST_ALTER_RACF_OPTIONS_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ALTER_RACF_OPTIONS_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = get_raw_sample(TEST_ALTER_RACF_OPTIONS_RESULT_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ALTER_RACF_OPTIONS_RESULT_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_alter_racf_options_parameter_errors() {
  test_validation_errors(TEST_ALTER_RACF_OPTIONS_PARAMETER_ERRORS_REQUEST_JSON,
                         TEST_ALTER_RACF_OPTIONS_PARAMETER_ERRORS_RESULT_JSON);
}

void test_parse_alter_racf_options_trait_errors() {
  test_validation_errors(TEST_ALTER_RACF_OPTIONS_TRAIT_ERRORS_REQUEST_JSON,
                         TEST_ALTER_RACF_OPTIONS_TRAIT_ERRORS_RESULT_JSON);
}

// Data-Set tests
void test_generate_add_data_set_request() {
  racfu_result_t result;
  std::string request_json   = get_json_sample(TEST_ADD_DATA_SET_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_ADD_DATA_SET_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ADD_DATA_SET_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_generate_alter_data_set_request() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ALTER_DATA_SET_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_ALTER_DATA_SET_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ALTER_DATA_SET_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_generate_delete_data_set_request() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_DELETE_DATA_SET_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_DELETE_DATA_SET_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_DELETE_DATA_SET_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_add_data_set_result() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ADD_DATA_SET_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ADD_DATA_SET_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = get_raw_sample(TEST_ADD_DATA_SET_RESULT_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ADD_DATA_SET_RESULT_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_delete_data_set_result() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_DELETE_DATA_SET_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_DELETE_DATA_SET_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = get_raw_sample(TEST_DELETE_DATA_SET_RESULT_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_DELETE_DATA_SET_RESULT_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_add_data_set_result_data_set_already_exists() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ADD_DATA_SET_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ADD_DATA_SET_RESULT_DATA_SET_ALREADY_EXISTS_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock =
      get_raw_sample(TEST_ADD_DATA_SET_RESULT_DATA_SET_ALREADY_EXISTS_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ADD_DATA_SET_RESULT_DATA_SET_ALREADY_EXISTS_RAW,
       &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_add_data_set_parameter_errors() {
  test_validation_errors(TEST_ADD_DATA_SET_PARAMETER_ERRORS_REQUEST_JSON,
                         TEST_ADD_DATA_SET_PARAMETER_ERRORS_RESULT_JSON);
}

void test_parse_add_data_set_trait_errors() {
  test_validation_errors(TEST_ADD_DATA_SET_TRAIT_ERRORS_REQUEST_JSON,
                         TEST_ADD_DATA_SET_TRAIT_ERRORS_RESULT_JSON);
}

// Resource tests
void test_generate_add_resource_request() {
  racfu_result_t result;
  std::string request_json   = get_json_sample(TEST_ADD_RESOURCE_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_ADD_RESOURCE_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ADD_RESOURCE_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_generate_alter_resource_request() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ALTER_RESOURCE_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_ALTER_RESOURCE_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ALTER_RESOURCE_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_generate_delete_resource_request() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_DELETE_RESOURCE_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_DELETE_RESOURCE_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_DELETE_RESOURCE_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_add_resource_result() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ADD_RESOURCE_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ADD_RESOURCE_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = get_raw_sample(TEST_ADD_RESOURCE_RESULT_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ADD_RESOURCE_RESULT_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_delete_resource_result() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_DELETE_RESOURCE_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_DELETE_RESOURCE_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = get_raw_sample(TEST_DELETE_RESOURCE_RESULT_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_DELETE_RESOURCE_RESULT_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_add_resource_result_resource_already_exists() {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_ADD_RESOURCE_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ADD_RESOURCE_RESULT_RESOURCE_ALREADY_EXISTS_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock =
      get_raw_sample(TEST_ADD_RESOURCE_RESULT_RESOURCE_ALREADY_EXISTS_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ADD_RESOURCE_RESULT_RESOURCE_ALREADY_EXISTS_RAW,
       &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_add_resource_parameter_errors() {
  test_validation_errors(TEST_ADD_RESOURCE_PARAMETER_ERRORS_REQUEST_JSON,
                         TEST_ADD_RESOURCE_PARAMETER_ERRORS_RESULT_JSON);
}

void test_parse_add_resource_trait_errors() {
  test_validation_errors(TEST_ADD_RESOURCE_TRAIT_ERRORS_REQUEST_JSON,
                         TEST_ADD_RESOURCE_TRAIT_ERRORS_RESULT_JSON);
}
// Permission tests
void test_generate_alter_permission_request() {
  racfu_result_t result;
  std::string request_json =
      get_json_sample(TEST_ALTER_PERMISSION_REQUEST_JSON);
  char *raw_request_expected =
      get_raw_sample(TEST_ALTER_PERMISSION_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ALTER_PERMISSION_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_generate_delete_permission_request() {
  racfu_result_t result;
  std::string request_json =
      get_json_sample(TEST_DELETE_PERMISSION_REQUEST_JSON);
  char *raw_request_expected =
      get_raw_sample(TEST_DELETE_PERMISSION_REQUEST_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_DELETE_PERMISSION_REQUEST_RAW, &raw_request_size_expected);

  // Mock IRRSMO64 result
  irrsmo64_result_mock      = NULL;
  irrsmo64_result_size_mock = 0;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_alter_permission_result() {
  racfu_result_t result;
  std::string request_json =
      get_json_sample(TEST_ALTER_PERMISSION_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_ALTER_PERMISSION_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = get_raw_sample(TEST_ALTER_PERMISSION_RESULT_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_ALTER_PERMISSION_RESULT_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_delete_permission_result() {
  racfu_result_t result;
  std::string request_json =
      get_json_sample(TEST_DELETE_PERMISSION_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_DELETE_PERMISSION_RESULT_JSON);

  // Mock IRRSMO64 result
  irrsmo64_result_mock = get_raw_sample(TEST_DELETE_PERMISSION_RESULT_RAW);
  struct stat raw_request_size_expected;
  stat(TEST_DELETE_PERMISSION_RESULT_RAW, &raw_request_size_expected);
  irrsmo64_result_size_mock = raw_request_size_expected.st_size;
  irrsmo64_saf_rc_mock      = 0;
  irrsmo64_racf_rc_mock     = 0;
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

void test_parse_alter_permission_parameter_errors() {
  test_validation_errors(TEST_ALTER_PERMISSION_PARAMETER_ERRORS_REQUEST_JSON,
                         TEST_ALTER_PERMISSION_PARAMETER_ERRORS_RESULT_JSON);
}

void test_parse_alter_permission_dataset_parameter_errors() {
  test_validation_errors(
      TEST_ALTER_PERMISSION_DATASET_PARAMETER_ERRORS_REQUEST_JSON,
      TEST_ALTER_PERMISSION_DATASET_PARAMETER_ERRORS_RESULT_JSON);
}

void test_parse_alter_permission_trait_errors() {
  test_validation_errors(TEST_ALTER_PERMISSION_TRAIT_ERRORS_REQUEST_JSON,
                         TEST_ALTER_PERMISSION_TRAIT_ERRORS_RESULT_JSON);
}
