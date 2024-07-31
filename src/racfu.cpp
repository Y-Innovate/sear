#include "racfu.hpp"

#include <stdint.h>

#include <nlohmann/json.hpp>
#include <string>

#include "extract.hpp"
#include "irrsmo00_conn.hpp"
#include "post_process.hpp"
#include "saf_xml_gen.hpp"
#include "saf_xml_parse.hpp"

void do_extract(const char *admin_type, const char *profile_name,
                const char *class_name, racfu_result_t *result,
                racfu_return_codes_t *return_codes);

void do_add_alter_delete(const char *admin_type, const char *profile_name,
                         const char *class_name, const char *operation,
                         nlohmann::json full_request_json,
                         racfu_result_t *results,
                         racfu_return_codes_t *return_codes);

void build_result(const char *operation, const char *admin_type,
                  const char *profile_name, const char *class_name,
                  char *raw_result, int raw_result_length, char *raw_request,
                  int raw_request_length, nlohmann::json profile_json,
                  racfu_result_t *result, racfu_return_codes_t *return_codes);

void racfu(racfu_result_t *result, char *request_json) {
  nlohmann::json request;
  std::string operation;
  request = nlohmann::json::parse(request_json);
  racfu_return_codes_t return_codes = {-1, -1, -1, -1, -1, -1};
  const char *profile_name = NULL;
  const char *class_name = NULL;
  // {
  //     "operation": "add",
  //     "admin_type": "user",
  //     "profile_name": "SQUIDWRD",
  //     "request_traits": {
  //        ...
  //     }
  // }
  // Extract
  operation = request["operation"].get<std::string>();
  if (operation.compare("extract") == 0) {
    if (request.contains("profile_name")) {
      profile_name = request["profile_name"].get<std::string>().c_str();
    }
    if (request.contains("class_name")) {
      class_name = request["class_name"].get<std::string>().c_str();
    }
    do_extract(request["admin_type"].get<std::string>().c_str(), profile_name,
               class_name, result, &return_codes);
    // Add/Alter/Delete
  } else {
    if (request.contains("profile_name")) {
      profile_name = request["profile_name"].get<std::string>().c_str();
    }
    if (request.contains("class_name")) {
      class_name = request["class_name"].get<std::string>().c_str();
    }
    do_add_alter_delete(request["admin_type"].get<std::string>().c_str(),
                        profile_name, class_name, operation.c_str(), request,
                        result, &return_codes);
  }
}

void do_extract(const char *admin_type, const char *profile_name,
                const char *class_name, racfu_result_t *racfu_result,
                racfu_return_codes_t *return_codes) {
  char *raw_result = NULL;
  char *raw_request = NULL;
  int raw_result_length, raw_request_length;
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
    raw_result = extract(profile_name, class_name, function_code, raw_request,
                         &raw_request_length, return_codes);
    if (raw_result == NULL) {
      return_codes->racfu_return_code = 4;
    } else {
      return_codes->racfu_return_code = 0;
    }
  }

  // Build Failure Result
  if (raw_result == NULL) {
    build_result("extract", admin_type, profile_name, class_name, nullptr, 0,
                 raw_request, raw_request_length, NULL, racfu_result,
                 return_codes);
    return;
  }

  // Post Process Generic Result
  if (strcmp(admin_type, "setropts") != 0) {
    generic_extract_parms_results_t *generic_result_buffer =
        (generic_extract_parms_results_t *)raw_result;
    raw_result_length = generic_result_buffer->result_buffer_length;
    profile_json = post_process_generic(generic_result_buffer);
    // Post Process Setropts Result
  } else {
    setropts_extract_results_t *setropts_result_buffer =
        (setropts_extract_results_t *)raw_result;
    raw_result_length = setropts_result_buffer->result_buffer_length;
    profile_json = post_process_setropts(setropts_result_buffer);
  }

  // Build Success Result
  build_result("extract", admin_type, profile_name, class_name, raw_result,
               raw_result_length, raw_request, raw_request_length, profile_json,
               racfu_result, return_codes);

  return;
}

void do_add_alter_delete(const char *admin_type, const char *profile_name,
                         const char *class_name, const char *operation,
                         nlohmann::json full_request_json,
                         racfu_result_t *racfu_result,
                         racfu_return_codes_t *return_codes) {
  char running_userid[8] = {0};
  char *xml_response_string, *xml_request_string;
  int irrsmo00_options, saf_rc, racf_rc, racf_rsn, racfu_rc;
  unsigned int result_buffer_size, request_length;
  bool debug_mode;

  nlohmann::json response_json;
  XmlParse *parser = new XmlParse();
  XmlGen *generator = new XmlGen();

  irrsmo00_options = 13;
  result_buffer_size = 10000;
  debug_mode = false;
  saf_rc = 0;
  racf_rc = 0;
  racf_rsn = 0;
  racfu_rc = 0;

  xml_request_string = generator->build_xml_string(
      full_request_json, running_userid, &irrsmo00_options, &result_buffer_size,
      &request_length, &racfu_rc, &debug_mode);

  if (racfu_rc != 0) {
    return_codes->racfu_return_code = racfu_rc;
    build_result(operation, admin_type, profile_name, class_name,
                 xml_request_string, request_length, nullptr, 0, NULL,
                 racfu_result, return_codes);
    return;
  }

  xml_response_string =
      call_irrsmo00(xml_request_string, running_userid, result_buffer_size,
                    irrsmo00_options, &saf_rc, &racf_rc, &racf_rsn, debug_mode);

  return_codes->saf_return_code = saf_rc;
  return_codes->racf_return_code = racf_rc;
  return_codes->racf_reason_code = racf_rsn;

  response_json =
      parser->build_json_string(xml_response_string, &racfu_rc, debug_mode);

  return_codes->racfu_return_code = racfu_rc;

  delete generator;
  delete parser;
  // free xml_response_string;
  // free json_res_string;
  // free xml_request_string;

  // Build Success Result
  build_result(operation, admin_type, profile_name, class_name,
               xml_response_string, result_buffer_size, xml_request_string,
               request_length, response_json, racfu_result, return_codes);
  // TODO: Make sure this isn't leaking memory?
  return;
}

void build_result(const char *operation, const char *admin_type,
                  const char *profile_name, const char *class_name,
                  char *raw_result, int raw_result_length, char *raw_request,
                  int raw_request_length, nlohmann::json profile_json,
                  racfu_result_t *racfu_result,
                  racfu_return_codes_t *return_codes) {
  // Build Return Code JSON
  nlohmann::json return_code_json = {
      {"return_codes",
       {{"saf_return_code", return_codes->saf_return_code},
        {"racf_return_code", return_codes->racf_return_code},
        {"racf_reason_code", return_codes->racf_reason_code},
        {"racfu_return_code", return_codes->racfu_return_code}}}};

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
  if (return_codes->racfu_return_code == -1) {
    return_code_json["return_codes"]["racfu_return_code"] = nullptr;
  }

  // Build Result JSON
  nlohmann::json result_json = {{"operation", operation},
                                {"admin_type", admin_type},
                                {"profile_name", profile_name},
                                {"result", profile_json},
                                {"return_codes", return_code_json}};
  if (profile_name == NULL) {
    result_json["profile_name"] = nullptr;
  }
  if (class_name != NULL) {
    result_json["class_name"] = class_name;
  }

  // Convert profile JSON to C string.
  std::string result_json_cpp_string = result_json.dump();
  char *result_json_string =
      (char *)malloc(sizeof(char) * (result_json_cpp_string.size() + 1));
  std::strcpy(result_json_string, result_json_cpp_string.c_str());

  // Build RACFu Result Structure
  racfu_result->raw_result = raw_result;
  racfu_result->raw_result_length = raw_result_length;
  racfu_result->raw_request = raw_request;
  racfu_result->raw_request_length = raw_request_length;
  racfu_result->result_json = result_json_string;

  return;
}