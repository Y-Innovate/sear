#include "security_admin.hpp"

#define _POSIX_C_SOURCE 200112L
#include <arpa/inet.h>

#include "extract.hpp"
#include "irrsmo00.hpp"
#include "messages.h"
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
  } catch (const nlohmann::json::parse_error &ex) {
    this->errors.add_racfu_error_message(
        "Syntax error in request JSON at byte " + std::to_string(ex.byte));
    this->return_codes.racfu_return_code = 8;
    this->build_result(nullptr, 0, nullptr, 0, {});
    return;
  }

  this->logger.debug(MSG_VALIDATING_PARAMETERS);
  try {
    parameter_validator.validate(this->request);
  } catch (const std::exception &ex) {
    this->errors.add_racfu_error_message(
        "The provided request JSON does not contain a valid request");
    this->return_codes.racfu_return_code = 8;
    this->build_result(nullptr, 0, nullptr, 0, {});
    return;
  }
  this->logger.debug(MSG_DONE);

  this->prepare();

  // Make Request To Corresponding Callable Service
  if (this->request["operation"].get<std::string>() == "extract") {
    this->logger.debug(MSG_SEQ_PATH);
    this->do_extract();
  } else {
    this->logger.debug(MSG_SMO_PATH);
    this->do_add_alter_delete();
  }
}

void SecurityAdmin::prepare() {
  this->admin_type = this->request["admin_type"].get<std::string>();
  this->operation  = this->request["operation"].get<std::string>();

  if (this->admin_type == "user") {
    this->function_code = USER_EXTRACT_FUNCTION_CODE;
    this->profile_name  = this->request["userid"].get<std::string>();
  } else if (this->admin_type == "group") {
    this->function_code = GROUP_EXTRACT_FUNCTION_CODE;
    this->profile_name  = this->request["group"].get<std::string>();
  } else if (this->admin_type == "group-connection") {
    this->function_code = GROUP_CONNECTION_EXTRACT_FUNCTION_CODE;
    if (this->operation == "extract") {
      this->profile_name = this->request["userid"].get<std::string>() + "." +
                           this->request["group"].get<std::string>();
    } else {
      this->profile_name = this->request["userid"].get<std::string>();
    }
  } else if (this->admin_type == "resource") {
    this->function_code = RESOURCE_EXTRACT_FUNCTION_CODE;
    this->profile_name  = this->request["resource"].get<std::string>();
    this->class_name    = this->request["class"].get<std::string>();
  } else if (this->admin_type == "data-set") {
    this->function_code = DATA_SET_EXTRACT_FUNCTION_CODE;
    this->profile_name  = this->request["data_set"].get<std::string>();
  } else if (this->admin_type == "racf-options") {
    this->function_code = SETROPTS_EXTRACT_FUNCTION_CODE;
  } else if (this->admin_type == "permission") {
    this->profile_name = this->request["resource"].get<std::string>();
    this->class_name   = this->request["class"].get<std::string>();
    this->auth_id      = this->request["userid"].get<std::string>();
  }
}

void SecurityAdmin::do_extract() {
  char *raw_result  = NULL;
  char *raw_request = NULL;
  int raw_result_length, raw_request_length;
  nlohmann::json profile_json;

  // Extract Profile
  raw_result = extract(this->profile_name, this->class_name,
                       this->function_code, &raw_request, raw_request_length,
                       this->return_codes, this->logger);
  if (raw_result == NULL) {
    this->return_codes.racfu_return_code = 4;
  } else {
    this->return_codes.racfu_return_code = 0;
  }

  // Build Failure Result
  if (raw_result == NULL) {
    if (this->admin_type != "racf-options") {
      this->errors.add_racfu_error_message("unable to extract '" +
                                           this->admin_type + "' profile '" +
                                           this->profile_name + "'");
    } else {
      this->errors.add_racfu_error_message("unable to extract '" +
                                           this->admin_type + "'");
    }
    this->build_result(raw_request, raw_request_length, nullptr, 0,
                       profile_json);
    return;
  }

  // Post Process Generic Result
  if (this->admin_type != "racf-options") {
    generic_extract_parms_results_t *generic_result_buffer =
        reinterpret_cast<generic_extract_parms_results_t *>(raw_result);
    raw_result_length = ntohl(generic_result_buffer->result_buffer_length);
    this->logger.debug(
        MSG_RESULT_SEQ_GENERIC,
        this->logger.cast_hex_string(raw_result, raw_result_length));
    profile_json =
        post_process_generic(generic_result_buffer, this->admin_type);
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
}

void SecurityAdmin::do_add_alter_delete() {
  char running_userid[8] = {0};
  char *xml_response_string, *xml_request_string;
  int irrsmo00_options            = 13;
  unsigned int result_buffer_size = 10000;
  unsigned int request_length;

  nlohmann::json intermediate_result_json;
  XmlParser parser       = XmlParser();
  XmlGenerator generator = XmlGenerator();

  xml_request_string     = generator.build_xml_string(
      this->admin_type, this->request, this->errors, this->profile_name,
      this->auth_id, running_userid, irrsmo00_options, request_length,
      this->logger);

  if (!this->errors.empty()) {
    this->return_codes.racfu_return_code = 8;
    this->build_result(xml_request_string, request_length, nullptr, 0, {});
    return;
  }

  if ((this->operation == "alter") &&
      ((this->admin_type == "group") || (this->admin_type == "user") ||
       (this->admin_type == "data-set") || (this->admin_type == "resource"))) {
    this->logger.debug(MSG_SMO_VALIDATE_EXIST);
    if (!does_profile_exist(this->admin_type, this->profile_name,
                            this->class_name, running_userid)) {
      if (class_name.empty()) {
        this->errors.add_racfu_error_message(
            "unable to alter '" + this->profile_name +
            "' because the profile does not exist");
      } else {
        this->errors.add_racfu_error_message(
            "Unable to alter '" + this->profile_name + "' in the '" +
            this->class_name + "' class because the profile does not exist");
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
      call_irrsmo00(xml_request_string, running_userid, result_buffer_size,
                    irrsmo00_options, this->return_codes);

  this->logger.debug(MSG_DONE);

  intermediate_result_json = parser.build_json_string(
      xml_response_string, this->return_codes.racfu_return_code, this->errors,
      this->logger);

  if (this->errors.empty()) {
    this->logger.debug(MSG_SMO_POST_PROCESS);
    this->logger.debug(intermediate_result_json.dump());
    // Maintain any RC 4's from parsing xml or post-processing json
    this->return_codes.racfu_return_code =
        this->return_codes.racfu_return_code |
        post_process_smo_json(this->errors, intermediate_result_json,
                              this->profile_name, this->admin_type,
                              this->class_name);
    this->logger.debug(MSG_DONE);
  }

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
