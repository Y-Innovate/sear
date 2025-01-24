#ifndef __RACFU_TEST_EXTRACT_H_
#define __RACFU_TEST_EXTRACT_H_

#ifdef __TOS_390__
// arg pointers will be 32-bit when we run on z/OS.
#define TEST_GENERIC_REQUEST_BUFFER_SIZE 1418
#define TEST_RACF_OPTIONS_REQUEST_BUFFER_SIZE 1372
#else
// arg pointers will be 64 bit when we run off platform.
#define TEST_GENERIC_REQUEST_BUFFER_SIZE 1474
#define TEST_RACF_OPTIONS_REQUEST_BUFFER_SIZE 1428
#endif

// arg area should be the same on every platform
#define TEST_GENERIC_ARG_AREA_SIZE 1362
#define TEST_RACF_OPTIONS_ARG_AREA_SIZE 1316

#define IRRSEQ00_REQUEST_SAMPLES "./tests/irrseq00/request_samples/"
#define IRRSEQ00_RESULT_SAMPLES "./tests/irrseq00/result_samples/"

// Request samples
// User
#define TEST_EXTRACT_USER_REQUEST_JSON \
  IRRSEQ00_REQUEST_SAMPLES "user/test_extract_user_request.json"
#define TEST_EXTRACT_USER_REQUEST_RAW \
  IRRSEQ00_REQUEST_SAMPLES "user/test_extract_user_request.bin"
#define TEST_EXTRACT_USER_REQUEST_REQUIRED_PARAMETER_MISSING_JSON \
  IRRSEQ00_REQUEST_SAMPLES                                        \
  "user/test_extract_user_request_required_parameter_missing.json"
#define TEST_EXTRACT_USER_REQUEST_EXTRANEOUS_PARAMETER_PROVIDED_JSON \
  IRRSEQ00_REQUEST_SAMPLES                                           \
  "user/test_extract_user_request_extraneous_parameter_provided.json"
// Group
#define TEST_EXTRACT_GROUP_REQUEST_JSON \
  IRRSEQ00_REQUEST_SAMPLES "group/test_extract_group_request.json"
#define TEST_EXTRACT_GROUP_REQUEST_RAW \
  IRRSEQ00_REQUEST_SAMPLES "group/test_extract_group_request.bin"
// Group Connection
#define TEST_EXTRACT_GROUP_CONNECTION_REQUEST_JSON \
  IRRSEQ00_REQUEST_SAMPLES                         \
  "group_connection/test_extract_group_connection_request.json"
#define TEST_EXTRACT_GROUP_CONNECTION_REQUEST_RAW \
  IRRSEQ00_REQUEST_SAMPLES                        \
  "group_connection/test_extract_group_connection_request.bin"
// RACF Options
#define TEST_EXTRACT_RACF_OPTIONS_REQUEST_JSON \
  IRRSEQ00_REQUEST_SAMPLES                     \
  "racf_options/"                              \
  "test_extract_racf_options_request.json"
#define TEST_EXTRACT_RACF_OPTIONS_REQUEST_RAW \
  IRRSEQ00_REQUEST_SAMPLES                    \
  "racf_options/"                             \
  "test_extract_racf_options_request.bin"
// Data Set
#define TEST_EXTRACT_DATA_SET_REQUEST_JSON \
  IRRSEQ00_REQUEST_SAMPLES "data_set/test_extract_data_set_request.json"
#define TEST_EXTRACT_DATA_SET_REQUEST_RAW \
  IRRSEQ00_REQUEST_SAMPLES "data_set/test_extract_data_set_request.bin"
// Resource
#define TEST_EXTRACT_RESOURCE_REQUEST_JSON \
  IRRSEQ00_REQUEST_SAMPLES "resource/test_extract_resource_request.json"
#define TEST_EXTRACT_RESOURCE_REQUEST_RAW \
  IRRSEQ00_REQUEST_SAMPLES "resource/test_extract_resource_request.bin"

// Result samples
// User
#define TEST_EXTRACT_USER_RESULT_JSON \
  IRRSEQ00_RESULT_SAMPLES "user/test_extract_user_result.json"
#define TEST_EXTRACT_USER_RESULT_RAW \
  IRRSEQ00_RESULT_SAMPLES "user/test_extract_user_result.bin"
#define TEST_EXTRACT_USER_RESULT_USER_NOT_FOUND_JSON \
  IRRSEQ00_RESULT_SAMPLES                            \
  "user/test_extract_user_result_user_not_found.json"
#define TEST_EXTRACT_USER_RESULT_REQUIRED_PARAMETER_MISSING_JSON \
  IRRSEQ00_RESULT_SAMPLES                                        \
  "user/test_extract_user_result_required_parameter_missing.json"
#define TEST_EXTRACT_USER_RESULT_EXTRANEOUS_PARAMETER_PROVIDED_JSON \
  IRRSEQ00_RESULT_SAMPLES                                           \
  "user/test_extract_user_result_extraneous_parameter_provided.json"
// Group
#define TEST_EXTRACT_GROUP_RESULT_JSON \
  IRRSEQ00_RESULT_SAMPLES "group/test_extract_group_result.json"
#define TEST_EXTRACT_GROUP_RESULT_RAW \
  IRRSEQ00_RESULT_SAMPLES "group/test_extract_group_result.bin"
#define TEST_EXTRACT_GROUP_RESULT_GROUP_NOT_FOUND_JSON \
  IRRSEQ00_RESULT_SAMPLES                              \
  "group/test_extract_group_result_group_not_found.json"
// Group Connection
#define TEST_EXTRACT_GROUP_CONNECTION_RESULT_JSON \
  IRRSEQ00_RESULT_SAMPLES                         \
  "group_connection/test_extract_group_connection_result.json"
#define TEST_EXTRACT_GROUP_CONNECTION_RESULT_RAW \
  IRRSEQ00_RESULT_SAMPLES                        \
  "group_connection/test_extract_group_connection_result.bin"
#define TEST_EXTRACT_GROUP_CONNECTION_RESULT_GROUP_CONNECTION_NOT_FOUND_JSON \
  IRRSEQ00_RESULT_SAMPLES                                                    \
  "group_connection/"                                                        \
  "test_extract_group_connection_result_group_connection_not_found.json"
// Racf Options
#define TEST_EXTRACT_RACF_OPTIONS_RESULT_JSON \
  IRRSEQ00_RESULT_SAMPLES "racf_options/test_extract_racf_options_result.json"
#define TEST_EXTRACT_RACF_OPTIONS_RESULT_RAW \
  IRRSEQ00_RESULT_SAMPLES "racf_options/test_extract_racf_options_result.bin"
#define TEST_EXTRACT_RACF_OPTIONS_RESULT_RACF_OPTIONS_NOT_FOUND_JSON \
  IRRSEQ00_RESULT_SAMPLES                                            \
  "racf_options/test_extract_racf_options_result_racf_options_not_found.json"
// Data Set
#define TEST_EXTRACT_DATA_SET_RESULT_JSON \
  IRRSEQ00_RESULT_SAMPLES "data_set/test_extract_data_set_result.json"
#define TEST_EXTRACT_DATA_SET_RESULT_RAW \
  IRRSEQ00_RESULT_SAMPLES "data_set/test_extract_data_set_result.bin"
#define TEST_EXTRACT_DATA_SET_RESULT_DATA_SET_NOT_FOUND_JSON \
  IRRSEQ00_RESULT_SAMPLES                                    \
  "data_set/test_extract_data_set_result_data_set_not_found.json"
// Resource
#define TEST_EXTRACT_RESOURCE_RESULT_JSON \
  IRRSEQ00_RESULT_SAMPLES "resource/test_extract_resource_result.json"
#define TEST_EXTRACT_RESOURCE_RESULT_RAW \
  IRRSEQ00_RESULT_SAMPLES "resource/test_extract_resource_result.bin"
#define TEST_EXTRACT_RESOURCE_RESULT_RESOURCE_NOT_FOUND_JSON \
  IRRSEQ00_RESULT_SAMPLES                                    \
  "resource/test_extract_resource_result_resource_not_found.json"

// Extract User
void test_generate_extract_user_request();
void test_parse_extract_user_result();
void test_parse_extract_user_result_user_not_found();
void test_parse_extract_user_result_required_parameter_missing();
void test_parse_extract_user_result_extraneous_parameter_provided();

// Extract Group
void test_generate_extract_group_request();
void test_parse_extract_group_result();
void test_parse_extract_group_result_group_not_found();

// Extract Group Connection
void test_generate_extract_group_connection_request();
void test_parse_extract_group_connection_result();
void test_parse_extract_group_connection_result_group_connection_not_found();

// Extract RACF Options
void test_generate_extract_racf_options_request();
void test_parse_extract_racf_options_result();
void test_parse_extract_racf_options_result_racf_options_not_found();

// Extract Data Set
void test_generate_extract_data_set_request();
void test_parse_extract_data_set_result();
void test_parse_extract_data_set_result_data_set_not_found();

// Extract Resource
void test_generate_extract_resource_request();
void test_parse_extract_resource_result();
void test_parse_extract_resource_result_resource_not_found();

#endif
