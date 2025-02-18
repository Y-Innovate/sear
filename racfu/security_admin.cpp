#include "security_admin.hpp"

#define _POSIX_C_SOURCE 200112L
#include <arpa/inet.h>

#include <cstdint>

#include "extract.hpp"
#include "irrsmo00.hpp"
#include "messages.h"
#include "parameter_validator.hpp"
#include "post_process.hpp"
#include "xml_generator.hpp"
#include "xml_parser.hpp"

namespace RACFu {
SecurityAdmin::SecurityAdmin(racfu_result_t *result, bool debug) {
  this->result       = result;

  this->logger       = Logger(debug);
  this->errors       = Errors();
  this->return_codes = {-1, -1, -1, -1};
}

void SecurityAdmin::make_request(const char *request_json) {
  // Parse Request JSON
  try {
    this->request = nlohmann::json::parse(request_json);
  } catch (nlohmann::json::parse_error &ex) {
    this->errors.add_racfu_error_message(
        "Syntax error in request JSON at byte " + std::to_string(ex.byte));
    this->return_codes.racfu_return_code = 8;
    this->build_result(nullptr, 0, nullptr, 0, {});
    return;
  }
  // Parameter Validation
  this->logger.debug(MSG_VALIDATING_PARAMETERS);
  ParameterValidator parameter_validator(&this->request, &this->errors);
  parameter_validator.validate_parameters();
  if (!errors.empty()) {
    return_codes.racfu_return_code = 8;
    this->build_result(nullptr, 0, nullptr, 0, {});
    return;
  }
  this->logger.debug(MSG_DONE);

  // Make Request To Corresponding Callable Service
  if (this->request["operation"].get<std::string>() == "extract") {
    this->logger.debug(MSG_SEQ_PATH);
    this->do_extract();
  } else {
    this->logger.debug(MSG_SMO_PATH);
    this->do_add_alter_delete();
  }
}

void SecurityAdmin::do_extract() {
  char *raw_result  = NULL;
  char *raw_request = NULL;
  int raw_result_length, raw_request_length;
  uint8_t function_code;
  nlohmann::json profile_json;
  std::string admin_type = this->request["admin_type"].get<std::string>();
  std::string profile_name;
  std::string class_name;

  // Validate 'admin_type' and build profile name and class name
  if (admin_type == "user") {
    function_code = USER_EXTRACT_FUNCTION_CODE;
    profile_name  = this->request["userid"].get<std::string>();
  } else if (admin_type == "group") {
    function_code = GROUP_EXTRACT_FUNCTION_CODE;
    profile_name  = this->request["group"].get<std::string>();
  } else if (admin_type == "group-connection") {
    function_code = GROUP_CONNECTION_EXTRACT_FUNCTION_CODE;
    profile_name  = this->request["userid"].get<std::string>() + "." +
                   this->request["group"].get<std::string>();
  } else if (admin_type == "resource") {
    function_code = RESOURCE_EXTRACT_FUNCTION_CODE;
    profile_name  = this->request["resource"].get<std::string>();
    class_name    = this->request["class"].get<std::string>();
  } else if (admin_type == "data-set") {
    function_code = DATA_SET_EXTRACT_FUNCTION_CODE;
    profile_name  = this->request["data_set"].get<std::string>();
  } else if (admin_type == "racf-options") {
    function_code = SETROPTS_EXTRACT_FUNCTION_CODE;
  } else {
    this->return_codes.racfu_return_code = 8;
  }

  // Do extract if function code is good.
  if (this->return_codes.racfu_return_code == -1) {
    raw_result =
        extract(profile_name, class_name, function_code, &raw_request,
                &raw_request_length, &this->return_codes, &this->logger);
    if (raw_result == NULL) {
      this->return_codes.racfu_return_code = 4;
    } else {
      this->return_codes.racfu_return_code = 0;
    }
  }

  // Build Failure Result
  if (raw_result == NULL) {
    if (admin_type != "racf-options") {
      this->errors.add_racfu_error_message("unable to extract '" + admin_type +
                                           "' profile '" + profile_name + "'");
    } else {
      this->errors.add_racfu_error_message("unable to extract '" + admin_type +
                                           "'");
    }
    this->build_result(raw_request, raw_request_length, nullptr, 0,
                       profile_json);
    return;
  }

  // Post Process Generic Result
  if (admin_type != "racf-options") {
    generic_extract_parms_results_t *generic_result_buffer =
        reinterpret_cast<generic_extract_parms_results_t *>(raw_result);
    raw_result_length = ntohl(generic_result_buffer->result_buffer_length);
    this->logger.debug(
        MSG_RESULT_SEQ_GENERIC,
        this->logger.cast_hex_string(raw_result, raw_result_length));
    profile_json =
        post_process_generic(generic_result_buffer, admin_type.c_str());
    // Post Process Setropts Result
  } else {
    setropts_extract_results_t *setropts_result_buffer =
        reinterpret_cast<setropts_extract_results_t *>(raw_result);
    raw_result_length = ntohl(setropts_result_buffer->result_buffer_length);
    this->logger.debug(
        MSG_RESULT_SEQ_SETROPTS,
        this->logger.cast_hex_string(raw_result, raw_result_length));
    profile_json = post_process_setropts(setropts_result_buffer);
  }

  this->logger.debug(MSG_SEQ_POST_PROCESS);

  // Build Success Result
  this->build_result(raw_request, raw_request_length, raw_result,
                     raw_result_length, profile_json);

  return;
}

void SecurityAdmin::do_add_alter_delete() {
  char running_userid[8] = {0};
  char *xml_response_string, *xml_request_string;
  int irrsmo00_options, saf_rc, racf_rc, racf_rsn, racfu_rc;
  unsigned int result_buffer_size, request_length;

  nlohmann::json intermediate_result_json;
  XmlParser parser       = XmlParser();
  XmlGenerator generator = XmlGenerator();

  irrsmo00_options       = 13;
  result_buffer_size     = 10000;
  saf_rc                 = 0;
  racf_rc                = 0;
  racf_rsn               = 0;
  racfu_rc               = 0;

  std::string admin_type = this->request["admin_type"].get<std::string>();
  std::string operation  = this->request["operation"].get<std::string>();
  std::string profile_name;
  std::string class_name;
  std::string auth_id;

  if (admin_type == "user" || admin_type == "group-connection") {
    profile_name = this->request["userid"].get<std::string>();
  } else if (admin_type == "group") {
    profile_name = this->request["group"].get<std::string>();
  } else if (admin_type == "resource") {
    profile_name = this->request["resource"].get<std::string>();
    class_name   = this->request["class"].get<std::string>();
  } else if (admin_type == "data-set") {
    profile_name = this->request["data_set"].get<std::string>();
  } else if (admin_type == "permission") {
    profile_name = this->request["resource"].get<std::string>();
    class_name   = this->request["class"].get<std::string>();
    auth_id      = this->request["userid"].get<std::string>();
  }

  xml_request_string = generator.build_xml_string(
      &admin_type, &this->request, this->errors, &profile_name, &auth_id,
      running_userid, &irrsmo00_options, &request_length, &this->logger);

  if (!this->errors.empty()) {
    this->return_codes.racfu_return_code = 8;
    this->build_result(xml_request_string, request_length, nullptr, 0, {});
    return;
  }

  if ((operation == "alter") &&
      ((admin_type == "group") || (admin_type == "user") ||
       (admin_type == "data-set") || (admin_type == "resource"))) {
    this->logger.debug(MSG_SMO_VALIDATE_EXIST);
    if (!does_profile_exist(admin_type, profile_name, class_name,
                            running_userid)) {
      if (class_name.empty()) {
        this->errors.add_racfu_error_message(
            "unable to alter '" + profile_name +
            "' because the profile does not exist");
      } else {
        this->errors.add_racfu_error_message(
            "Unable to alter '" + profile_name + "' in the '" + class_name +
            "' class because the profile does not exist");
      }
    }
    if (!this->errors.empty()) {
      this->return_codes.racfu_return_code = 8;
      this->build_result(xml_request_string, request_length, nullptr, 0, {});
      return;
    }
    this->logger.debug(MSG_DONE);
  }

  this->logger.debug(MSG_CALLING_SMO);

  xml_response_string =
      call_irrsmo00(xml_request_string, running_userid, &result_buffer_size,
                    irrsmo00_options, &saf_rc, &racf_rc, &racf_rsn);

  this->logger.debug(MSG_DONE);

  this->return_codes.saf_return_code  = saf_rc;
  this->return_codes.racf_return_code = racf_rc;
  this->return_codes.racf_reason_code = racf_rsn;

  intermediate_result_json            = parser.build_json_string(
      xml_response_string, &racfu_rc, this->errors, &this->logger);

  if (this->errors.empty()) {
    this->logger.debug(MSG_SMO_POST_PROCESS);
    this->logger.debug(intermediate_result_json.dump());
    // Maintain any RC 4's from parsing xml or post-processing json
    racfu_rc = racfu_rc |
               post_process_smo_json(this->errors, &intermediate_result_json,
                                     &profile_name, &admin_type, &class_name);
    this->logger.debug(MSG_DONE);
  }

  this->return_codes.racfu_return_code = racfu_rc;

  // Build Success Result
  this->build_result(xml_request_string, request_length, xml_response_string,
                     result_buffer_size, intermediate_result_json);
}

void SecurityAdmin::build_result(
    char *raw_request, int raw_request_length, char *raw_result,
    int raw_result_length, const nlohmann::json &intermediate_result_json) {
  this->logger.debug(MSG_BUILD_RESULT);
  // Build Result JSON starting with Return Codes
  nlohmann::json result_json = {
      {"return_codes",
       {{"saf_return_code", this->return_codes.saf_return_code},
        {"racf_return_code", this->return_codes.racf_return_code},
        {"racf_reason_code", this->return_codes.racf_reason_code},
        {"racfu_return_code", this->return_codes.racfu_return_code}}}
  };

  // Convert '-1' to 'nullptr'
  if (this->return_codes.saf_return_code == -1) {
    result_json["return_codes"]["saf_return_code"] = nullptr;
  }
  if (this->return_codes.racf_return_code == -1) {
    result_json["return_codes"]["racf_return_code"] = nullptr;
  }
  if (this->return_codes.racf_reason_code == -1) {
    result_json["return_codes"]["racf_reason_code"] = nullptr;
  }
  if (this->return_codes.racf_return_code == -1) {
    result_json["return_codes"]["racf_return_code"] = nullptr;
  }
  if (this->return_codes.racfu_return_code == -1) {
    result_json["return_codes"]["racfu_return_code"] = nullptr;
  }

  if (!this->errors.empty()) {
    result_json["errors"] = this->errors.error_messages;
  }

  if (intermediate_result_json != nullptr && this->errors.empty()) {
    if (!intermediate_result_json.empty()) {
      result_json.merge_patch(intermediate_result_json);
    }
  }

  // Convert profile JSON to C string.
  std::string result_json_cpp_string = result_json.dump();
  char *result_json_string           = static_cast<char *>(
      malloc(sizeof(char) * (result_json_cpp_string.size() + 1)));
  std::strcpy(result_json_string, result_json_cpp_string.c_str());

  // Build RACFu Result Structure
  this->result->raw_result         = raw_result;
  this->result->raw_result_length  = raw_result_length;
  this->result->raw_request        = raw_request;
  this->result->raw_request_length = raw_request_length;
  this->result->result_json        = result_json_string;

  this->logger.debug(MSG_DONE);
}
}  // namespace RACFu
