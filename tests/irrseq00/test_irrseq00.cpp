#include "tests/irrseq00/test_irrseq00.hpp"

#define _POSIX_C_SOURCE 200112L
#include <arpa/inet.h>
#include <sys/stat.h>

#include <cstring>

#include "tests/unit_test_utilities.hpp"

/*************************************************************************/
/* User                                                                  */
/*************************************************************************/
void test_generate_extract_user_request() {
  test_extract_request_generation(TEST_EXTRACT_USER_REQUEST_JSON,
                                  TEST_EXTRACT_USER_REQUEST_RAW, false);
}

void test_parse_extract_user_result() {
  test_parse_extract_result(TEST_EXTRACT_USER_REQUEST_JSON,
                            TEST_EXTRACT_USER_RESULT_JSON,
                            TEST_EXTRACT_USER_RESULT_RAW);
}

void test_parse_extract_user_result_user_not_found() {
  test_parse_extract_result_profile_not_found(
      TEST_EXTRACT_USER_REQUEST_JSON,
      TEST_EXTRACT_USER_RESULT_USER_NOT_FOUND_JSON);
}

void test_parse_extract_user_result_required_parameter_missing() {
  test_validation_errors(
      TEST_EXTRACT_USER_REQUEST_REQUIRED_PARAMETER_MISSING_JSON,
      TEST_EXTRACT_USER_RESULT_REQUIRED_PARAMETER_MISSING_JSON);
}

void test_parse_extract_user_result_extraneous_parameter_provided() {
  test_validation_errors(
      TEST_EXTRACT_USER_REQUEST_EXTRANEOUS_PARAMETER_PROVIDED_JSON,
      TEST_EXTRACT_USER_RESULT_EXTRANEOUS_PARAMETER_PROVIDED_JSON);
}

/*************************************************************************/
/* Group                                                                 */
/*************************************************************************/
void test_generate_extract_group_request() {
  test_extract_request_generation(TEST_EXTRACT_GROUP_REQUEST_JSON,
                                  TEST_EXTRACT_GROUP_REQUEST_RAW, false);
}

void test_parse_extract_group_result() {
  test_parse_extract_result(TEST_EXTRACT_GROUP_REQUEST_JSON,
                            TEST_EXTRACT_GROUP_RESULT_JSON,
                            TEST_EXTRACT_GROUP_RESULT_RAW);
}

void test_parse_extract_group_result_group_not_found() {
  test_parse_extract_result_profile_not_found(
      TEST_EXTRACT_GROUP_REQUEST_JSON,
      TEST_EXTRACT_GROUP_RESULT_GROUP_NOT_FOUND_JSON);
}

void test_parse_extract_group_result_required_parameter_missing() {
  test_validation_errors(
      TEST_EXTRACT_GROUP_REQUEST_REQUIRED_PARAMETER_MISSING_JSON,
      TEST_EXTRACT_GROUP_RESULT_REQUIRED_PARAMETER_MISSING_JSON);
}

void test_parse_extract_group_result_extraneous_parameter_provided() {
  test_validation_errors(
      TEST_EXTRACT_GROUP_REQUEST_EXTRANEOUS_PARAMETER_PROVIDED_JSON,
      TEST_EXTRACT_GROUP_RESULT_EXTRANEOUS_PARAMETER_PROVIDED_JSON);
}

/*************************************************************************/
/* Group Connection                                                      */
/*************************************************************************/
void test_generate_extract_group_connection_request() {
  test_extract_request_generation(TEST_EXTRACT_GROUP_CONNECTION_REQUEST_JSON,
                                  TEST_EXTRACT_GROUP_CONNECTION_REQUEST_RAW,
                                  false);
}

void test_parse_extract_group_connection_result() {
  test_parse_extract_result(TEST_EXTRACT_GROUP_CONNECTION_REQUEST_JSON,
                            TEST_EXTRACT_GROUP_CONNECTION_RESULT_JSON,
                            TEST_EXTRACT_GROUP_CONNECTION_RESULT_RAW);
}

void test_parse_extract_group_connection_result_group_connection_not_found() {
  test_parse_extract_result_profile_not_found(
      TEST_EXTRACT_GROUP_CONNECTION_REQUEST_JSON,
      TEST_EXTRACT_GROUP_CONNECTION_RESULT_GROUP_CONNECTION_NOT_FOUND_JSON);
}

void test_parse_extract_group_connection_result_required_parameter_missing() {
  test_validation_errors(
      TEST_EXTRACT_GROUP_CONNECTION_REQUEST_REQUIRED_PARAMETER_MISSING_JSON,
      TEST_EXTRACT_GROUP_CONNECTION_RESULT_REQUIRED_PARAMETER_MISSING_JSON);
}

void test_parse_extract_group_connection_result_extraneous_parameter_provided() {
  test_validation_errors(
      TEST_EXTRACT_GROUP_CONNECTION_REQUEST_EXTRANEOUS_PARAMETER_PROVIDED_JSON,
      TEST_EXTRACT_GROUP_CONNECTION_RESULT_EXTRANEOUS_PARAMETER_PROVIDED_JSON);
}

/*************************************************************************/
/* RACF Options                                                          */
/*************************************************************************/
void test_generate_extract_racf_options_request() {
  test_extract_request_generation(TEST_EXTRACT_RACF_OPTIONS_REQUEST_JSON,
                                  TEST_EXTRACT_RACF_OPTIONS_REQUEST_RAW, true);
}

void test_parse_extract_racf_options_result() {
  test_parse_extract_result(TEST_EXTRACT_RACF_OPTIONS_REQUEST_JSON,
                            TEST_EXTRACT_RACF_OPTIONS_RESULT_JSON,
                            TEST_EXTRACT_RACF_OPTIONS_RESULT_RAW);
}

void test_parse_extract_racf_options_result_racf_options_not_found() {
  // This would technically never fail this way, but this tests
  // the code paths for handling an error with RACF Options extract.
  test_parse_extract_result_profile_not_found(
      TEST_EXTRACT_RACF_OPTIONS_REQUEST_JSON,
      TEST_EXTRACT_RACF_OPTIONS_RESULT_RACF_OPTIONS_NOT_FOUND_JSON);
}

void test_parse_extract_racf_options_result_extraneous_parameter_provided() {
  test_validation_errors(
      TEST_EXTRACT_RACF_OPTIONS_REQUEST_EXTRANEOUS_PARAMETER_PROVIDED_JSON,
      TEST_EXTRACT_RACF_OPTIONS_RESULT_EXTRANEOUS_PARAMETER_PROVIDED_JSON);
}

/*************************************************************************/
/* Data Set                                                              */
/*************************************************************************/
void test_generate_extract_data_set_request() {
  test_extract_request_generation(TEST_EXTRACT_DATA_SET_REQUEST_JSON,
                                  TEST_EXTRACT_DATA_SET_REQUEST_RAW, false);
}

void test_parse_extract_data_set_result() {
  test_parse_extract_result(TEST_EXTRACT_DATA_SET_REQUEST_JSON,
                            TEST_EXTRACT_DATA_SET_RESULT_JSON,
                            TEST_EXTRACT_DATA_SET_RESULT_RAW);
}

void test_parse_extract_data_set_result_data_set_not_found() {
  test_parse_extract_result_profile_not_found(
      TEST_EXTRACT_DATA_SET_REQUEST_JSON,
      TEST_EXTRACT_DATA_SET_RESULT_DATA_SET_NOT_FOUND_JSON);
}

void test_parse_extract_data_set_result_required_parameter_missing() {
  test_validation_errors(
      TEST_EXTRACT_DATA_SET_REQUEST_REQUIRED_PARAMETER_MISSING_JSON,
      TEST_EXTRACT_DATA_SET_RESULT_REQUIRED_PARAMETER_MISSING_JSON);
}

void test_parse_extract_data_set_result_extraneous_parameter_provided() {
  test_validation_errors(
      TEST_EXTRACT_DATA_SET_REQUEST_EXTRANEOUS_PARAMETER_PROVIDED_JSON,
      TEST_EXTRACT_DATA_SET_RESULT_EXTRANEOUS_PARAMETER_PROVIDED_JSON);
}

/*************************************************************************/
/* Resource                                                              */
/*************************************************************************/
void test_generate_extract_resource_request() {
  test_extract_request_generation(TEST_EXTRACT_RESOURCE_REQUEST_JSON,
                                  TEST_EXTRACT_RESOURCE_REQUEST_RAW, false);
}

void test_parse_extract_resource_result() {
  test_parse_extract_result(TEST_EXTRACT_RESOURCE_REQUEST_JSON,
                            TEST_EXTRACT_RESOURCE_RESULT_JSON,
                            TEST_EXTRACT_RESOURCE_RESULT_RAW);
}

void test_parse_extract_resource_result_resource_not_found() {
  test_parse_extract_result_profile_not_found(
      TEST_EXTRACT_RESOURCE_REQUEST_JSON,
      TEST_EXTRACT_RESOURCE_RESULT_RESOURCE_NOT_FOUND_JSON);
}

void test_parse_extract_resource_result_required_parameter_missing() {
  test_validation_errors(
      TEST_EXTRACT_RESOURCE_REQUEST_REQUIRED_PARAMETER_MISSING_JSON,
      TEST_EXTRACT_RESOURCE_RESULT_REQUIRED_PARAMETER_MISSING_JSON);
}

void test_parse_extract_resource_result_extraneous_parameter_provided() {
  test_validation_errors(
      TEST_EXTRACT_RESOURCE_REQUEST_EXTRANEOUS_PARAMETER_PROVIDED_JSON,
      TEST_EXTRACT_RESOURCE_RESULT_EXTRANEOUS_PARAMETER_PROVIDED_JSON);
}
