#include "racfu_result.hpp"

#include "extract.hpp"
#include "post_process.hpp"

#include <nlohmann/json.hpp>

#include <stdio.h>
#include <string.h>
#include <iostream>

// todo use C++ strings instead here?
void do_extract(
    racfu_result_t *result, 
    const char *admin_type, 
    const char *profile_name
) {
  char *result_buffer;

  // Do Profile Extract
  if (strcmp(admin_type, "user") == 0) {
    result_buffer = extract(
        profile_name,
        NULL,
        USER_EXTRACT_FUNCTION_CODE);
  } else if (strcmp(admin_type, "group") == 0) {
    result_buffer = extract(
        profile_name,
        NULL,
        GROUP_EXTRACT_FUNCTION_CODE);
  } else if (strcmp(admin_type, "group-connection") == 0) {
    result_buffer = extract(
        profile_name,
        NULL,
        GROUP_CONNECTION_EXTRACT_FUNCTION_CODE);
  } else if (strcmp(admin_type, "resource") == 0) {
    result_buffer = extract(
        profile_name,
        argv[3],
        RESOURCE_EXTRACT_FUNCTION_CODE);
  } else if (strcmp(admin_type, "data-set") == 0) {
    result_buffer = extract(
        profile_name,
        NULL,
        DATA_SET_EXTRACT_FUNCTION_CODE);
  } else if (strcmp(admin_type, "setropts") == 0) {
    result_buffer = extract(
        NULL,
        NULL,
        SETROPTS_EXTRACT_FUNCTION_CODE);
  } else {
    std::cout << "Bad Extract Function: " << admin_type << std::endl;
    return 1;
  }

  // Check Result
  if (result_buffer == NULL) {
    return 1;
  }

  int result_buffer_length;
  nlohmann::json profile;

  // Post Process Generic Result
  if (strcmp(admin_type, "setropts") != 0) {
    generic_extract_parms_results_t *generic_result_buffer =
        (generic_extract_parms_results_t *) result_buffer;
    result_buffer_length = generic_result_buffer->result_buffer_length;
    profile = post_process_generic(generic_result_buffer);
    // Post Process Setropts Result
  } else {
    setropts_extract_results_t *setropts_result_buffer =
        (setropts_extract_results_t *) result_buffer;
    result_buffer_length = setropts_result_buffer->result_buffer_length;
    profile = post_process_setropts(setropts_result_buffer);
  }

  // Print profile JSON
  std::cout << profile.dump(2) << std::endl;

  // Temporary raw dump for testing.
  FILE *fp = fopen("extract.bin", "wb");
  fwrite(result_buffer, result_buffer_length, 1, fp);
  fclose(fp);

  free(result_buffer);

  return 0;
}
