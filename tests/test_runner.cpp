#include "irrseq00.hpp"
#include "tests/irrseq00/test_irrseq00.hpp"
#include "tests/irrsmo00/test_irrsmo00.hpp"
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
  // User
  RUN_TEST(test_generate_add_user_request);
  RUN_TEST(test_generate_alter_user_request);
  RUN_TEST(test_generate_delete_user_request);
  RUN_TEST(test_parse_add_user_result);
  RUN_TEST(test_parse_delete_user_result);
  RUN_TEST(test_parse_add_user_result_user_already_exists);
  RUN_TEST(test_parse_add_user_parameter_errors);
  RUN_TEST(test_parse_add_user_trait_errors);
  RUN_TEST(test_parse_add_user_no_xml_data_error);
  RUN_TEST(test_parse_alter_user_no_xml_data_error);
  RUN_TEST(test_parse_alter_user_traits_not_json_error);
  RUN_TEST(test_parse_irrsmo00_errors_result);
  RUN_TEST(test_parse_delete_user_trait_error_result);
  // Group
  RUN_TEST(test_generate_add_group_request);
  RUN_TEST(test_generate_alter_group_request);
  RUN_TEST(test_generate_delete_group_request);
  RUN_TEST(test_parse_add_group_result);
  RUN_TEST(test_parse_delete_group_result);
  RUN_TEST(test_parse_add_group_result_group_already_exists);
  RUN_TEST(test_parse_add_group_parameter_errors);
  RUN_TEST(test_parse_add_group_trait_errors);
  // Group Connection
  RUN_TEST(test_generate_alter_group_connection_request);
  RUN_TEST(test_generate_delete_group_connection_request);
  RUN_TEST(test_parse_alter_group_connection_result);
  RUN_TEST(test_parse_delete_group_connection_result);
  RUN_TEST(test_parse_alter_group_connection_parameter_errors);
  RUN_TEST(test_parse_alter_group_connection_trait_errors);
  // Racf Options
  RUN_TEST(test_generate_alter_racf_options_request);
  RUN_TEST(test_parse_alter_racf_options_result);
  RUN_TEST(test_parse_alter_racf_options_parameter_errors);
  RUN_TEST(test_parse_alter_racf_options_trait_errors);
  // Data-Set
  RUN_TEST(test_generate_add_data_set_request);
  RUN_TEST(test_generate_alter_data_set_request);
  RUN_TEST(test_generate_delete_data_set_request);
  RUN_TEST(test_parse_add_data_set_result);
  RUN_TEST(test_parse_delete_data_set_result);
  RUN_TEST(test_parse_add_data_set_result_data_set_already_exists);
  RUN_TEST(test_parse_add_data_set_parameter_errors);
  RUN_TEST(test_parse_add_data_set_trait_errors);
  // Resource
  RUN_TEST(test_generate_add_resource_request);
  RUN_TEST(test_generate_alter_resource_request);
  RUN_TEST(test_generate_delete_resource_request);
  RUN_TEST(test_parse_add_resource_result);
  RUN_TEST(test_parse_delete_resource_result);
  RUN_TEST(test_parse_add_resource_result_resource_already_exists);
  RUN_TEST(test_parse_add_resource_parameter_errors);
  RUN_TEST(test_parse_add_resource_trait_errors);
  // Permission
  RUN_TEST(test_generate_alter_permission_request);
  RUN_TEST(test_generate_delete_permission_request);
  RUN_TEST(test_parse_alter_permission_result);
  RUN_TEST(test_parse_delete_permission_result);
  RUN_TEST(test_parse_alter_permission_parameter_errors);
  RUN_TEST(test_parse_alter_permission_trait_errors);

  // Profile Extract
  RUN_TEST(test_generate_extract_user_request);
  RUN_TEST(test_parse_extract_user_result);
  RUN_TEST(test_parse_extract_user_result_user_not_found);

  return UNITY_END();
}
