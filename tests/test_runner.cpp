#include "irrseq00.hpp"
#include "tests/test_extract.hpp"
#include "tests/unity/unity.h"

void setUp(void) {}

void tearDown(void) {}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_generate_extract_user_request);
  RUN_TEST(test_parse_extract_user_result);
  RUN_TEST(test_parse_extract_user_result_user_not_found);

  return UNITY_END();
}