#include "irrseq00.hpp"
#include "tests/common/test_common_errors.hpp"
#include "tests/irrseq00/test_extract.hpp"
#include "tests/irrsmo00/test_add.hpp"
#include "tests/unity/unity.h"

void setUp(void) {}

void tearDown(void) {}

int main(void) {
  UNITY_BEGIN();

  // Common
  RUN_TEST(test_parse_parameters_junk_error);
  RUN_TEST(test_parse_parameters_missing_error);
  RUN_TEST(test_parse_parameters_nonstring_error);

  // Add
  RUN_TEST(test_generate_add_user_request);
  RUN_TEST(test_parse_add_user_result);
  RUN_TEST(test_parse_add_user_result_user_already_exists);

  // Profile Extract
  RUN_TEST(test_generate_extract_user_request);
  RUN_TEST(test_parse_extract_user_result);
  RUN_TEST(test_parse_extract_user_result_user_not_found);

  return UNITY_END();
}