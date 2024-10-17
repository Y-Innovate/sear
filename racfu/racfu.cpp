#include "racfu.h"

#include <stdint.h>

#include <nlohmann/json.hpp>
#include <string>

#include "extract.hpp"
#include "irrsmo00.hpp"
#include "post_process.hpp"
#include "xml_generator.hpp"
#include "xml_parser.hpp"

void do_extract(const char *admin_type, const char *profile_name,
                const char *class_name, racfu_result_t *result,
                racfu_return_codes_t *return_codes);

void do_add_alter_delete(const char *admin_type, const char *profile_name,
                         const char *class_name, const char *operation,
                         const char *surrogate_userid,
                         nlohmann::json full_request_json,
                         racfu_result_t *results,
                         racfu_return_codes_t *return_codes);

void build_result(const char *operation, const char *admin_type,
                  const char *profile_name, const char *class_name,
                  const char *surrogate_userid, char *raw_result,
                  int raw_result_length, char *raw_request,
                  int raw_request_length, nlohmann::json profile_json,
                  racfu_result_t *result, racfu_return_codes_t *return_codes);

void racfu(racfu_result_t *result, const char *request_json) {
  nlohmann::json request, errors;
  std::string operation, admin_type;
  request = nlohmann::json::parse(request_json);
  racfu_return_codes_t return_codes = {-1, -1, -1, -1};
  const char *profile_name = NULL;
  const char *class_name = NULL;
  const char *surrogate_userid = NULL;
  // {
  //     "operation": "add",
  //     "admin_type": "user",
  //     "profile_name": "SQUIDWRD",
  //     "traits": {
  //        ...
  //     }
  // }
  // Extract
  if (!request.contains("operation")) {
    update_error_json(&errors, "missing_header_attribute", "operation");
    operation = "";
  }
  if (!request.contains("admin_type")) {
    update_error_json(&errors, "missing_header_attribute", "admin_type");
    admin_type = "";
  }
  if (!errors.empty()) {
    return_codes.racfu_return_code = 8;
    build_result(operation.c_str(), admin_type.c_str(), profile_name,
                 class_name, NULL, nullptr, 0, nullptr, 0, errors, result,
                 &return_codes);
    return;
  }
  operation = request["operation"].get<std::string>();
  admin_type = request["admin_type"].get<std::string>();
  if (request.contains("profile_name")) {
    profile_name = request["profile_name"].get<std::string>().c_str();
  }
  if (request.contains("class_name")) {
    class_name = request["class_name"].get<std::string>().c_str();
  }
  if (operation.compare("extract") == 0) {
    do_extract(admin_type.c_str(), profile_name, class_name, result,
               &return_codes);
    // Add/Alter/Delete
  } else {
    if (request.contains("running_user_id")) {
      surrogate_userid = request["running_user_id"].get<std::string>().c_str();
    }
    do_add_alter_delete(admin_type.c_str(), profile_name, class_name,
                        operation.c_str(), surrogate_userid, request, result,
                        &return_codes);
  }
}

void do_extract(const char *admin_type, const char *profile_name,
                const char *class_name, racfu_result_t *racfu_result,
                racfu_return_codes_t *return_codes) {
  char *raw_result = NULL;
  char *raw_request = NULL;
  int raw_result_length, raw_request_length;
  uint8_t function_code;
  nlohmann::json profile_json, errors;

  if (profile_name == NULL) {
    update_error_json(&errors, "missing_header_attribute", "profile_name");
  }

  // Validate 'admin_type'
  if (strcmp(admin_type, "user") == 0) {
    function_code = USER_EXTRACT_FUNCTION_CODE;
  } else if (strcmp(admin_type, "group") == 0) {
    function_code = GROUP_EXTRACT_FUNCTION_CODE;
  } else if (strcmp(admin_type, "group-connection") == 0) {
    function_code = GROUP_CONNECTION_EXTRACT_FUNCTION_CODE;
  } else if (strcmp(admin_type, "resource") == 0) {
    function_code = RESOURCE_EXTRACT_FUNCTION_CODE;
    if (class_name == NULL) {
      update_error_json(&errors, "missing_header_attribute", "class_name");
    }
  } else if (strcmp(admin_type, "data-set") == 0) {
    function_code = DATA_SET_EXTRACT_FUNCTION_CODE;
  } else if (strcmp(admin_type, "setropts") == 0) {
    function_code = SETROPTS_EXTRACT_FUNCTION_CODE;
  } else {
    return_codes->racfu_return_code = 8;
    update_error_json(&errors, "bad_header_value",
                      "admin_type:" + std::string(admin_type));
  }

  if (!errors.empty()) {
    return_codes->racfu_return_code = 8;
    build_result("extract", admin_type, profile_name, class_name, NULL, nullptr,
                 0, nullptr, 0, errors, racfu_result, return_codes);
    return;
  }

  // Do extract if function code is good.
  if (return_codes->racfu_return_code == -1) {
    raw_result = extract(profile_name, class_name, function_code, &raw_request,
                         &raw_request_length, return_codes);
    if (raw_result == NULL) {
      return_codes->racfu_return_code = 4;
    } else {
      return_codes->racfu_return_code = 0;
    }
  }

  // Build Failure Result
  if (raw_result == NULL) {
    build_result("extract", admin_type, profile_name, class_name, NULL, nullptr,
                 0, raw_request, raw_request_length, errors, racfu_result,
                 return_codes);
    return;
  }

  // Post Process Generic Result
  if (strcmp(admin_type, "setropts") != 0) {
    generic_extract_parms_results_t *generic_result_buffer =
        reinterpret_cast<generic_extract_parms_results_t *>(raw_result);
    raw_result_length = generic_result_buffer->result_buffer_length;
    profile_json = post_process_generic(generic_result_buffer);
    // Post Process Setropts Result
  } else {
    setropts_extract_results_t *setropts_result_buffer =
        reinterpret_cast<setropts_extract_results_t *>(raw_result);
    raw_result_length = setropts_result_buffer->result_buffer_length;
    profile_json = post_process_setropts(setropts_result_buffer);
  }

  // Build Success Result
  build_result("extract", admin_type, profile_name, class_name, NULL,
               raw_result, raw_result_length, raw_request, raw_request_length,
               profile_json, racfu_result, return_codes);

  return;
}

void do_add_alter_delete(const char *admin_type, const char *profile_name,
                         const char *class_name, const char *operation,
                         const char *surrogate_userid,
                         nlohmann::json full_request_json,
                         racfu_result_t *racfu_result,
                         racfu_return_codes_t *return_codes) {
  char running_userid[8] = {0};
  char *xml_response_string, *xml_request_string;
  int irrsmo00_options, saf_rc, racf_rc, racf_rsn, racfu_rc;
  unsigned int result_buffer_size, request_length;
  bool debug_mode;

  nlohmann::json response_json, errors;
  XmlParser *parser = new XmlParser();
  XmlGenerator *generator = new XmlGenerator();

  irrsmo00_options = 13;
  result_buffer_size = 10000;
  debug_mode = false;
  saf_rc = 0;
  racf_rc = 0;
  racf_rsn = 0;
  racfu_rc = 0;

  xml_request_string = generator->build_xml_string(
      admin_type, full_request_json, &errors, running_userid, &irrsmo00_options,
      &result_buffer_size, &request_length, &debug_mode);

  if (!errors.empty()) {
    racfu_rc = 8;
    return_codes->racfu_return_code = racfu_rc;
    build_result(operation, admin_type, profile_name, class_name,
                 surrogate_userid, nullptr, 0, xml_request_string,
                 request_length, errors, racfu_result, return_codes);
    return;
  }

  xml_response_string =
      call_irrsmo00(xml_request_string, running_userid, &result_buffer_size,
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
               surrogate_userid, xml_response_string, result_buffer_size,
               xml_request_string, request_length, response_json, racfu_result,
               return_codes);
  // TODO: Make sure this isn't leaking memory?
  return;
}

void build_result(const char *operation, const char *admin_type,
                  const char *profile_name, const char *class_name,
                  const char *surrogate_userid, char *raw_result,
                  int raw_result_length, char *raw_request,
                  int raw_request_length, nlohmann::json profile_json,
                  racfu_result_t *racfu_result,
                  racfu_return_codes_t *return_codes) {
  // Build Return Code JSON
  nlohmann::json return_code_json = {
      {"return_codes",
       {{"saf_return_code", return_codes->saf_return_code},
        {"racf_return_code", return_codes->racf_return_code},
        {"racf_reason_code", return_codes->racf_reason_code},
        {"racfu_return_code", return_codes->racfu_return_code}}}
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
  if (return_codes->racfu_return_code == -1) {
    return_code_json["return_codes"]["racfu_return_code"] = nullptr;
  }

  // Build Result JSON
  nlohmann::json result_json = {
      {   "operation",        operation},
      {  "admin_type",       admin_type},
      {"profile_name",     profile_name},
      {"return_codes", return_code_json}
  };
  if (profile_name == NULL) {
    result_json["profile_name"] = nullptr;
  }
  if (class_name != NULL) {
    result_json["class_name"] = class_name;
  }
  if (surrogate_userid != NULL) {
    if (strlen(surrogate_userid) != 0) {
      result_json["surrogate_userid"] = surrogate_userid;
    }
  }

  if (profile_json.contains("errors")) {
    std::string error_message_str;
    result_json["result"];
    for (auto &error_type : profile_json["errors"].items()) {
      for (auto &error_focus : error_type.value().items()) {
        error_message_str = "RACFu encountered a " + error_type.key() +
                            " error while working with " +
                            error_focus.value().get<std::string>() +
                            ". If you supplied this as part of your input "
                            "json, you may need to re-examine this item.";
        result_json["result"] += error_message_str;
      }
    }
  } else {
    result_json["result"] = profile_json;
  }

  // Convert profile JSON to C string.
  std::string result_json_cpp_string = result_json.dump();
  char *result_json_string = static_cast<char *>(
      malloc(sizeof(char) * (result_json_cpp_string.size() + 1)));
  std::strcpy(result_json_string, result_json_cpp_string.c_str());

  // Build RACFu Result Structure
  racfu_result->raw_result = raw_result;
  racfu_result->raw_result_length = raw_result_length;
  racfu_result->raw_request = raw_request;
  racfu_result->raw_request_length = raw_request_length;
  racfu_result->result_json = result_json_string;

  return;
}