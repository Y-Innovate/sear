#include "extract.hpp"
#include "post_process.hpp"

#include <nlohmann/json.hpp>

#include <stdio.h>
#include <string.h>
#include <iostream>

using json = nlohmann::json;

int main(int argc, char **argv)
{
  char *result_buffer;

  // Do Profile Extract
  if (strcmp(argv[1], "user") == 0) {
    result_buffer = extract(
        argv[2],
        NULL,
        USER_EXTRACT_FUNCTION_CODE);
  } else if (strcmp(argv[1], "group") == 0) {
    result_buffer = extract(
        argv[2],
        NULL,
        GROUP_EXTRACT_FUNCTION_CODE);
  } else if (strcmp(argv[1], "group-connection") == 0) {
    result_buffer = extract(
        argv[2],
        NULL,
        GROUP_CONNECTION_EXTRACT_FUNCTION_CODE);
  } else if (strcmp(argv[1], "resource") == 0) {
    result_buffer = extract(
        argv[2],
        argv[3],
        RESOURCE_EXTRACT_FUNCTION_CODE);
  } else if (strcmp(argv[1], "data-set") == 0) {
    result_buffer = extract(
        argv[2],
        NULL,
        DATA_SET_EXTRACT_FUNCTION_CODE);
  } else if (strcmp(argv[1], "setropts") == 0) {
    result_buffer = extract(
        NULL,
        NULL,
        SETROPTS_EXTRACT_FUNCTION_CODE);
  } else {
    std::cout << "Bad Extract Function: " << argv[1] << std::endl;
    return 1;
  }

  // Check Result
  if (result_buffer == NULL) {
    return 1;
  }

  int result_buffer_length;
  json profile;

  // Post Process Generic Result
  if (strcmp(argv[1], "setropts") != 0) {
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
