#include "racfu.hpp"

#include "irrsmo00_conn.hpp"
#include "extract.hpp"
#include "post_process.hpp"

#include <nlohmann/json.hpp>

#include <stdint.h>
#include <string>

void do_extract(
    const char *admin_type,
    const char *profile_name,
    const char *class_name,
    racfu_result_t *result,
    racfu_return_codes_t *return_codes);

void build_result(
    const char *operation,
    const char *admin_type,
    char *raw_result,
    nlohmann::json profile_json,
    racfu_result_t *result,
    racfu_return_codes_t *return_codes);

void racfu(racfu_result_t *result, char *request_json) {
  nlohmann::json request;
  request = nlohmann::json::parse(request_json);
  racfu_return_codes_t return_codes = { -1, -1, -1, -1, -1, -1 };
  const char *class_name = NULL;
  // {
  //     "operation": "add",
  //     "admin_type": "user",
  //     "profile_name": "SQUIDWRD",
  //     "request_data": {
  //        ...
  //     }
  // }
  // Extract
  if (request["operation"].get<std::string>().compare("extract") == 0) {
    if (request.contains("class_name")) {
      class_name = request["class_name"].get<std::string>().c_str();
    }
    do_extract(
        request["admin_type"].get<std::string>().c_str(),
        request["profile_name"].get<std::string>().c_str(),
        class_name,
        result,
        &return_codes);
  // Add/Alter/Delete
  } else {
    // todo just pass the json object.
    call_irrsmo00_with_json(request_json, result);
  }
}

void do_extract(
    const char *admin_type,
    const char *profile_name,
    const char *class_name,
    racfu_result_t *result,
    racfu_return_codes_t *return_codes
) {
  char *result_buffer = NULL;
  int result_buffer_length;
  uint8_t function_code;
  nlohmann::json profile_json;

  // Validate 'admin_type'
  if (strcmp(admin_type, "user") == 0) {
    function_code = USER_EXTRACT_FUNCTION_CODE;
  } else if (strcmp(admin_type, "group") == 0) {
    function_code = GROUP_EXTRACT_FUNCTION_CODE;
  } else if (strcmp(admin_type, "group-connection") == 0) {
    function_code = GROUP_CONNECTION_EXTRACT_FUNCTION_CODE;
  } else if (strcmp(admin_type, "resource") == 0) {
    function_code = RESOURCE_EXTRACT_FUNCTION_CODE;
  } else if (strcmp(admin_type, "data-set") == 0) {
    function_code = DATA_SET_EXTRACT_FUNCTION_CODE;
  } else if (strcmp(admin_type, "setropts") == 0) {
    function_code = SETROPTS_EXTRACT_FUNCTION_CODE;
  } else {
    return_codes->racfu_return_code = 8;
  }

  // Do extract if function code is good.
  if (return_codes->racfu_return_code == -1) {
    result_buffer = extract(
        profile_name,
        class_name,
        function_code,
        return_codes);
    if (result_buffer == NULL) {
      return_codes->racfu_return_code = 4;
    } else {
      return_codes->racfu_return_code = 0;
    }
  }

  // Build Failure Result
  if (result_buffer == NULL) {
    build_result(
        "extract",
        admin_type,
        NULL,
        nullptr,
        result,
        return_codes);
    return;
  }

  // Post Process Generic Result
  if (strcmp(admin_type, "setropts") != 0) {
    generic_extract_parms_results_t *generic_result_buffer =
        (generic_extract_parms_results_t *) result_buffer;
    result_buffer_length = generic_result_buffer->result_buffer_length;
    profile_json = post_process_generic(generic_result_buffer);
  // Post Process Setropts Result
  } else {
    setropts_extract_results_t *setropts_result_buffer =
        (setropts_extract_results_t *) result_buffer;
    result_buffer_length = setropts_result_buffer->result_buffer_length;
    profile_json = post_process_setropts(setropts_result_buffer);
  }

  // Build Success Result
  build_result(
      "extract",
      admin_type,
      result_buffer,
      profile_json,
      result,
      return_codes);

  return;
}

void build_result(
    const char *operation,
    const char *admin_type,
    char *raw_result,
    nlohmann::json profile_json,
    racfu_result_t *result,
    racfu_return_codes_t *return_codes
) {
  // Build Return Code JSON
  nlohmann::json return_code_json = {
    {"return_codes", {
      {"saf_return_code", return_codes->saf_return_code},
      {"racf_return_code", return_codes->racf_return_code},
      {"racf_reason_code", return_codes->racf_reason_code},
      {"irrseq00_return_code", return_codes->irrseq00_return_code},
      {"irrsmo00_return_code", return_codes->irrsmo00_return_code},
      {"racfu_return_code", return_codes->racfu_return_code}
    }}
  };

  // Convert '-1' to 'nullptr'
  if (return_codes->saf_return_code == -1) {
    return_code_json["return_codes"]["saf_return_code"] = nullptr;
  }
  if (return_codes->racf_return_code == -1) {
    return_code_json["return_codes"]["racf_return_code"] = nullptr;
  } 
  if (return_codes->racf_reason_code == -1) {
    return_code_json["return_codes"]["racf_reason_code"] = nullptr;
  } 
  if (return_codes->racf_return_code == -1) {
    return_code_json["return_codes"]["racf_return_code"] = nullptr;
  }
  if (return_codes->irrsmo00_return_code == -1) {
    return_code_json["return_codes"]["irrsmo00_return_code"] = nullptr;
  }
  if (return_codes->irrseq00_return_code == -1) {
    return_code_json["return_codes"]["irrseq00_return_code"] = nullptr;
  }
  if (return_codes->racfu_return_code == -1) {
    return_code_json["return_codes"]["racfu_return_code"] = nullptr;
  }

  // Build Result JSON
  nlohmann::json result_json = {
    {"operation", operation},
    {"admin_type", admin_type},
    {"result", profile_json},
    {"return_codes", return_code_json}
  };

  // Convert profile JSON to C string.
  std::string result_json_cpp_string = result_json.dump();
  char *result_json_string = new char[result_json_cpp_string.size() + 1];
  std::strcpy(result_json_string, result_json_cpp_string.c_str());

  // Build RACFu Result Structure
  result->raw_result = raw_result;
  result->result_json = result_json_string;

  return;
}