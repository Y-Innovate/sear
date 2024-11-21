#include "irrseq00.hpp"
#include "tests/irrseq00/test_extract.hpp"
#include "tests/irrsmo00/test_add.hpp"
#include "tests/unity/unity.h"
#include "tests/validation/test_parameter_validation.hpp"

void setUp() {}

void tearDown() {}

int main() {
  UNITY_BEGIN();

  // Validation
  RUN_TEST(test_parse_parameters_junk_error);
  RUN_TEST(test_parse_parameters_missing_error);
  RUN_TEST(test_parse_parameters_nonstring_error);

  // IRRSMO00
  RUN_TEST(test_generate_add_user_request);
  RUN_TEST(test_parse_add_user_result);
  RUN_TEST(test_parse_add_user_result_user_already_exists);
  RUN_TEST(test_parse_add_user_parameter_errors);
  RUN_TEST(test_parse_add_user_trait_errors);
  RUN_TEST(test_parse_add_user_no_xml_data_error);
  RUN_TEST(test_parse_alter_user_no_xml_data_error);
  RUN_TEST(test_parse_alter_user_traits_not_json_error);
  RUN_TEST(test_parse_irrsmo00_errors_result);

  // Profile Extract
  RUN_TEST(test_generate_extract_user_request);
  RUN_TEST(test_parse_extract_user_result);
  RUN_TEST(test_parse_extract_user_result_user_not_found);

  return UNITY_END();
}
