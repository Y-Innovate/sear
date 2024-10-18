#include "tests/irrseq00/test_extract.hpp"

#include <string.h>
#include <sys/stat.h>

#include "racfu/racfu.h"
#include "tests/mock/irrseq00.hpp"
#include "tests/unit_test_utilities.hpp"
#include "tests/unity/unity.h"

void check_arg_pointers(char *raw_request) {
  // Arg Pointers on z/Architecture (31-bit big endian)
  /*
  0x00, 0x00, 0x00, 0x00, // result buffer pointer
  0x29, 0x96, 0x90, 0x48, // work area pointer
  0x29, 0x96, 0x94, 0x48, // ALET pointer
  0x29, 0x96, 0x94, 0x4c, // SAF return code pointer
  0x29, 0x96, 0x94, 0x50, // ALET pointer
  0x29, 0x96, 0x94, 0x54, // RACF return code pointer
  0x29, 0x96, 0x94, 0x58, // ALET pointer
  0x29, 0x96, 0x94, 0x5c, // RACF reason code pointer
  0x29, 0x96, 0x94, 0x60, // function code pointer
  0x29, 0x96, 0x94, 0x61, // profile extract parms pointer
  0x29, 0x96, 0x94, 0x9d, // profile name pointer
  0x29, 0x96, 0x95, 0x95, // ACEE pointer
  0x29, 0x96, 0x95, 0x99, // result buffer subpool pointer
  0xa9, 0x96, 0x95, 0x9a // result buffer pointer pointer
  */

  // Arg Pointers on x86_64/ARM64 (64-bit little endian)
  /*
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // result buffer pointer
  0x00, 0x88, 0x00, 0x35, 0x01, 0x00, 0x00, 0x00, // work area pointer
  0x00, 0x8c, 0x00, 0x35, 0x01, 0x00, 0x00, 0x00, // ALET pointer
  0x04, 0x8c, 0x00, 0x35, 0x01, 0x00, 0x00, 0x00, // SAF return code pointer
  0x08, 0x8c, 0x00, 0x35, 0x01, 0x00, 0x00, 0x00, // ALET pointer
  0x0c, 0x8c, 0x00, 0x35, 0x01, 0x00, 0x00, 0x00, // RACF return code pointer
  0x10, 0x8c, 0x00, 0x35, 0x01, 0x00, 0x00, 0x00, // ALET pointer
  0x14, 0x8c, 0x00, 0x35, 0x01, 0x00, 0x00, 0x00, // RACF reason code pointer
  0x18, 0x8c, 0x00, 0x35, 0x01, 0x00, 0x00, 0x00, // function code pointer
  0x19, 0x8c, 0x00, 0x35, 0x01, 0x00, 0x00, 0x00, // profile extract parms
  pointer 0x55, 0x8c, 0x00, 0x35, 0x01, 0x00, 0x00, 0x00, // profile name
  pointer 0x4d, 0x8d, 0x00, 0x35, 0x01, 0x00, 0x00, 0x00, // ACEE pointer 0x51,
  0x8d, 0x00, 0x35, 0x01, 0x00, 0x00, 0x00, // result buffer subpool pointer
  0x52, 0x8d, 0x00, 0xb5, 0x01, 0x00, 0x00, 0x00 // result buffer pointer
  pointer
  */

#ifdef __TOS_390__
  uint32_t *arg_pointer =
      (uint32_t *)(raw_request + TEST_GENERIC_ARG_AREA_SIZE);
#else
  uint64_t *arg_pointer =
      (uint64_t *)(raw_request + TEST_GENERIC_ARG_AREA_SIZE);
#endif
  // Result buffer pointer should be left NULL since no
  // result buffer exists until after R_Admin is called.
  TEST_ASSERT_EQUAL_UINT64(0, arg_pointer[0]);
  // work area pointer should be set.
  TEST_ASSERT_NOT_EQUAL_UINT64(0, arg_pointer[1]);
  // work area should be 1024 bytes
  TEST_ASSERT_EQUAL_UINT64(1024, arg_pointer[2] - arg_pointer[1]);
  // SAF RC ALET should be 4 bytes
  TEST_ASSERT_EQUAL_UINT64(4, arg_pointer[3] - arg_pointer[2]);
  // SAF RC should be 4 bytes
  TEST_ASSERT_EQUAL_UINT64(4, arg_pointer[4] - arg_pointer[3]);
  // RACF RC ALET should be 4 bytes
  TEST_ASSERT_EQUAL_UINT64(4, arg_pointer[5] - arg_pointer[4]);
  // RACF RC should be 4 bytes
  TEST_ASSERT_EQUAL_UINT64(4, arg_pointer[6] - arg_pointer[5]);
  // RACF reason ALET should be 4 bytes
  TEST_ASSERT_EQUAL_UINT64(4, arg_pointer[7] - arg_pointer[6]);
  // RACF reason should be 4 bytes
  TEST_ASSERT_EQUAL_UINT64(4, arg_pointer[8] - arg_pointer[7]);
  // function code pointer should be 1 byte
  TEST_ASSERT_EQUAL_UINT64(1, arg_pointer[9] - arg_pointer[8]);
  // profile extract parms area should be 61 bytes
  TEST_ASSERT_EQUAL_UINT64(61, arg_pointer[10] - arg_pointer[8]);
  // profile name area should be 248 bytes
  TEST_ASSERT_EQUAL_UINT64(248, arg_pointer[11] - arg_pointer[10]);
  // ACEE should be 4 bytes
  TEST_ASSERT_EQUAL_UINT64(4, arg_pointer[12] - arg_pointer[11]);
  // result buffer subpool should be 1 byte
  // Note that the difference between the result buffer pointer pointer
  // and the result buffer subpool pointer is 0x80000001 as a result
  // of the high order bit of the result buffer pointer pointer being
  // set to 0x80000000 to indicate that this is the end of the argument list.
  TEST_ASSERT_EQUAL_UINT64(0x80000001, arg_pointer[13] - arg_pointer[12]);
}

void test_generate_extract_user_request(void) {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_EXTRACT_USER_REQUEST_JSON);
  char *raw_request_expected = get_raw_sample(TEST_EXTRACT_USER_REQUEST_RAW);

  // Mock R_Admin result
  r_admin_result_mock = NULL;
  r_admin_result_size_mock = 0;
  r_admin_rc_mock = 0;
  r_admin_saf_rc_mock = 0;
  r_admin_racf_rc_mock = 0;
  r_admin_racf_reason_mock = 0;

  racfu(&result, request_json.c_str());

  // Check the size of the buffer
  TEST_ASSERT_EQUAL_INT32(TEST_GENERIC_REQUEST_BUFFER_SIZE,
                          result.raw_request_length);
  // Check the "arg area" (excludes the "arg pointers" at the end)
  TEST_ASSERT_EQUAL_MEMORY(raw_request_expected, result.raw_request,
                           TEST_GENERIC_ARG_AREA_SIZE);

  check_arg_pointers(result.raw_request);

  // Cleanup
  free(raw_request_expected);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}

void test_parse_extract_user_result(void) {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_EXTRACT_USER_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_EXTRACT_USER_RESULT_JSON);

  // Mock R_Admin result
  r_admin_result_mock = get_raw_sample(TEST_EXTRACT_USER_RESULT_RAW);
  struct stat st;
  stat(TEST_EXTRACT_USER_RESULT_RAW, &st);
  r_admin_result_size_mock = st.st_size;
  r_admin_rc_mock = 0;
  r_admin_saf_rc_mock = 0;
  r_admin_racf_rc_mock = 0;
  r_admin_racf_reason_mock = 0;

  racfu(&result, request_json.c_str());

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);
  TEST_ASSERT_EQUAL_INT32(result_json_expected.length(),
                          strlen(result.result_json));

  // Cleanup
  free(r_admin_result_mock);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}

void test_parse_extract_user_result_user_not_found(void) {
  racfu_result_t result;
  std::string request_json = get_json_sample(TEST_EXTRACT_USER_REQUEST_JSON);
  std::string result_json_expected =
      get_json_sample(TEST_EXTRACT_USER_RESULT_USER_NOT_FOUND_JSON);

  // Mock R_Admin result
  // Note that there will be no result if the profile cannot be extracted
  // and the return and reason codes will be set to indicate why the extract
  // failed.
  r_admin_result_mock = NULL;
  r_admin_result_size_mock = 0;
  r_admin_rc_mock = -1;
  r_admin_saf_rc_mock = 4;
  r_admin_racf_rc_mock = 4;
  r_admin_racf_reason_mock = 4;

  racfu(&result, request_json.c_str());

  TEST_ASSERT_EQUAL_STRING(result_json_expected.c_str(), result.result_json);
  TEST_ASSERT_EQUAL_INT32(result_json_expected.length(),
                          strlen(result.result_json));

  // Cleanup
  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);
}