#include "racfu.h"

#define _POSIX_C_SOURCE 200112L
#include <arpa/inet.h>

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#include "errors.hpp"
#include "extract.hpp"
#include "irrsmo00.hpp"
#include "logger.hpp"
#include "messages.h"
#include "parameter_validator.hpp"
#include "post_process.hpp"
#include "xml_generator.hpp"
#include "xml_parser.hpp"

void do_extract(nlohmann::json *request, racfu_result_t *result,
                racfu_return_codes_t *return_codes_p, Logger *logger_p);

void do_add_alter_delete(nlohmann::json *request, racfu_result_t *result,
                         racfu_return_codes_t *return_codes_p,
                         Logger *logger_p);

void build_result(std::string operation, char *raw_result,
                  int raw_result_length, char *raw_request,
                  int raw_request_length, RACFu::Errors &errors,
                  nlohmann::json *intermediate_result_json,
                  racfu_result_t *result, racfu_return_codes_t *return_codes_p,
                  Logger *logger_p);

void racfu(racfu_result_t *result, const char *request_json, bool debug) {
  Logger logger = Logger(debug);
  RACFu::Errors errors;
  nlohmann::json request;
  racfu_return_codes_t return_codes = {-1, -1, -1, -1};
  try {
    request = nlohmann::json::parse(request_json);
  } catch (nlohmann::json::parse_error &ex) {
    errors.add_racfu_error_message("Syntax error in request JSON at byte " +
                                   std::to_string(ex.byte));
    return_codes.racfu_return_code = 8;
    build_result("", nullptr, 0, nullptr, 0, errors, {}, result, &return_codes,
                 &logger);
    return;
  }
  logger.debug(MSG_VALIDATING_PARAMETERS);
  RACFu::ParameterValidator parameter_validator(&request, &errors);
  parameter_validator.validate_parameters();
  if (!errors.empty()) {
    return_codes.racfu_return_code = 8;
    build_result("", nullptr, 0, nullptr, 0, errors, {}, result, &return_codes,
                 &logger);
    return;
  }
  logger.debug(MSG_DONE);

  // Extract
  if (request["operation"].get<std::string>() == "extract") {
    logger.debug(MSG_SEQ_PATH);
    do_extract(&request, result, &return_codes, &logger);
    // Add/Alter/Delete
  } else {
    logger.debug(MSG_SMO_PATH);
    do_add_alter_delete(&request, result, &return_codes, &logger);
  }
}

void do_extract(nlohmann::json *request, racfu_result_t *racfu_result,
                racfu_return_codes_t *return_codes_p, Logger *logger_p) {
  char *raw_result  = NULL;
  char *raw_request = NULL;
  int raw_result_length, raw_request_length;
  uint8_t function_code;
  nlohmann::json profile_json;
  std::string admin_type = (*request)["admin_type"].get<std::string>();
  std::string profile_name;
  std::string class_name;

  // Validate 'admin_type' and build profile name and class name
  if (admin_type == "user") {
    function_code = USER_EXTRACT_FUNCTION_CODE;
    profile_name  = (*request)["userid"].get<std::string>();
  } else if (admin_type == "group") {
    function_code = GROUP_EXTRACT_FUNCTION_CODE;
    profile_name  = (*request)["group"].get<std::string>();
  } else if (admin_type == "group-connection") {
    function_code = GROUP_CONNECTION_EXTRACT_FUNCTION_CODE;
    profile_name  = (*request)["userid"].get<std::string>() + "." +
                   (*request)["group"].get<std::string>();
  } else if (admin_type == "resource") {
    function_code = RESOURCE_EXTRACT_FUNCTION_CODE;
    profile_name  = (*request)["resource"].get<std::string>();
    class_name    = (*request)["class"].get<std::string>();
  } else if (admin_type == "data-set") {
    function_code = DATA_SET_EXTRACT_FUNCTION_CODE;
    profile_name  = (*request)["data_set"].get<std::string>();
  } else if (admin_type == "racf-options") {
    function_code = SETROPTS_EXTRACT_FUNCTION_CODE;
  } else {
    return_codes_p->racfu_return_code = 8;
  }

  // Do extract if function code is good.
  if (return_codes_p->racfu_return_code == -1) {
    raw_result =
        extract(&profile_name, &class_name, function_code, &raw_request,
                &raw_request_length, return_codes_p, logger_p);
    if (raw_result == NULL) {
      return_codes_p->racfu_return_code = 4;
    } else {
      return_codes_p->racfu_return_code = 0;
    }
  }

  RACFu::Errors errors;

  // Build Failure Result
  if (raw_result == NULL) {
    if (admin_type != "racf-options") {
      errors.add_racfu_error_message("unable to extract '" + admin_type +
                                     "' profile '" + profile_name + "'");
    } else {
      errors.add_racfu_error_message("unable to extract '" + admin_type + "'");
    }
    build_result((*request)["operation"].get<std::string>(), nullptr, 0,
                 raw_request, raw_request_length, errors, &profile_json,
                 racfu_result, return_codes_p, logger_p);
    return;
  }

  // Post Process Generic Result
  if (admin_type != "racf-options") {
    generic_extract_parms_results_t *generic_result_buffer =
        reinterpret_cast<generic_extract_parms_results_t *>(raw_result);
    raw_result_length = ntohl(generic_result_buffer->result_buffer_length);
    logger_p->debug(MSG_RESULT_SEQ_GENERIC,
                    logger_p->cast_hex_string(raw_result, raw_result_length));
    profile_json =
        post_process_generic(generic_result_buffer, admin_type.c_str());
    // Post Process Setropts Result
  } else {
    setropts_extract_results_t *setropts_result_buffer =
        reinterpret_cast<setropts_extract_results_t *>(raw_result);
    raw_result_length = ntohl(setropts_result_buffer->result_buffer_length);
    logger_p->debug(MSG_RESULT_SEQ_SETROPTS,
                    logger_p->cast_hex_string(raw_result, raw_result_length));
    profile_json = post_process_setropts(setropts_result_buffer);
  }

  logger_p->debug(MSG_SEQ_POST_PROCESS);

  // Build Success Result
  build_result((*request)["operation"].get<std::string>(), raw_result,
               raw_result_length, raw_request, raw_request_length, errors,
               &profile_json, racfu_result, return_codes_p, logger_p);

  return;
}

void do_add_alter_delete(nlohmann::json *request, racfu_result_t *result,
                         racfu_return_codes_t *return_codes_p,
                         Logger *logger_p) {
  char running_userid[8] = {0};
  char *xml_response_string, *xml_request_string;
  int irrsmo00_options, saf_rc, racf_rc, racf_rsn, racfu_rc;
  unsigned int result_buffer_size, request_length;

  RACFu::Errors errors;
  nlohmann::json intermediate_result_json;
  XmlParser *parser       = new XmlParser();
  XmlGenerator *generator = new XmlGenerator();

  irrsmo00_options        = 13;
  result_buffer_size      = 10000;
  saf_rc                  = 0;
  racf_rc                 = 0;
  racf_rsn                = 0;
  racfu_rc                = 0;

  std::string admin_type  = (*request)["admin_type"].get<std::string>();
  std::string operation   = (*request)["operation"].get<std::string>();
  std::string profile_name;
  std::string class_name;
  std::string auth_id;

  if (admin_type == "user" || admin_type == "group-connection") {
    profile_name = (*request)["userid"].get<std::string>();
  } else if (admin_type == "group") {
    profile_name = (*request)["group"].get<std::string>();
  } else if (admin_type == "resource") {
    profile_name = (*request)["resource"].get<std::string>();
    class_name   = (*request)["class"].get<std::string>();
  } else if (admin_type == "data-set") {
    profile_name = (*request)["data_set"].get<std::string>();
  } else if (admin_type == "permission") {
    profile_name = (*request)["resource"].get<std::string>();
    class_name   = (*request)["class"].get<std::string>();
    auth_id      = (*request)["userid"].get<std::string>();
  }

  xml_request_string = generator->build_xml_string(
      &admin_type, request, errors, &profile_name, &auth_id, running_userid,
      &irrsmo00_options, &request_length, logger_p);

  if (!errors.empty()) {
    return_codes_p->racfu_return_code = 8;
    build_result(operation, nullptr, 0, xml_request_string, request_length,
                 errors, {}, result, return_codes_p, logger_p);
    return;
  }

  if ((operation == "alter") &&
      ((admin_type == "group") || (admin_type == "user") ||
       (admin_type == "data-set") || (admin_type == "resource"))) {
    logger_p->debug(MSG_SMO_VALIDATE_EXIST);
    if (!does_profile_exist(admin_type, profile_name, class_name,
                            running_userid)) {
      if (class_name.empty()) {
        errors.add_racfu_error_message("unable to alter '" + profile_name +
                                       "' because the profile does not exist");
      } else {
        errors.add_racfu_error_message(
            "Unable to alter '" + profile_name + "' in the '" + class_name +
            "' class because the profile does not exist");
      }
    }
    if (!errors.empty()) {
      return_codes_p->racfu_return_code = 8;
      build_result(operation, nullptr, 0, xml_request_string, request_length,
                   errors, {}, result, return_codes_p, logger_p);
      return;
    }
    logger_p->debug(MSG_DONE);
  }

  logger_p->debug(MSG_CALLING_SMO);

  xml_response_string =
      call_irrsmo00(xml_request_string, running_userid, &result_buffer_size,
                    irrsmo00_options, &saf_rc, &racf_rc, &racf_rsn);

  logger_p->debug(MSG_DONE);

  return_codes_p->saf_return_code  = saf_rc;
  return_codes_p->racf_return_code = racf_rc;
  return_codes_p->racf_reason_code = racf_rsn;

  intermediate_result_json         = parser->build_json_string(
      xml_response_string, &racfu_rc, errors, logger_p);

  if (errors.empty()) {
    logger_p->debug(MSG_SMO_POST_PROCESS);
    logger_p->debug(intermediate_result_json.dump());
    // Maintain any RC 4's from parsing xml or post-processing json
    racfu_rc = racfu_rc |
               post_process_smo_json(errors, &intermediate_result_json,
                                     &profile_name, &admin_type, &class_name);
    logger_p->debug(MSG_DONE);
  }

  return_codes_p->racfu_return_code = racfu_rc;

  delete generator;
  delete parser;
  // free xml_response_string;
  // free json_res_string;
  // free xml_request_string;

  // Build Success Result
  build_result(operation, xml_response_string, result_buffer_size,
               xml_request_string, request_length, errors,
               &intermediate_result_json, result, return_codes_p, logger_p);
  // TODO: Make sure this isn't leaking memory?
  return;
}

void build_result(std::string operation, char *raw_result,
                  int raw_result_length, char *raw_request,
                  int raw_request_length, RACFu::Errors &errors,
                  nlohmann::json *intermediate_result_json,
                  racfu_result_t *racfu_result,
                  racfu_return_codes_t *return_codes_p, Logger *logger_p) {
  logger_p->debug(MSG_BUILD_RESULT);
  // Build Result JSON starting with Return Codes
  nlohmann::json result_json = {
      {"return_codes",
       {{"saf_return_code", return_codes_p->saf_return_code},
        {"racf_return_code", return_codes_p->racf_return_code},
        {"racf_reason_code", return_codes_p->racf_reason_code},
        {"racfu_return_code", return_codes_p->racfu_return_code}}}
  };

  // Convert '-1' to 'nullptr'
  if (return_codes_p->saf_return_code == -1) {
    result_json["return_codes"]["saf_return_code"] = nullptr;
  }
  if (return_codes_p->racf_return_code == -1) {
    result_json["return_codes"]["racf_return_code"] = nullptr;
  }
  if (return_codes_p->racf_reason_code == -1) {
    result_json["return_codes"]["racf_reason_code"] = nullptr;
  }
  if (return_codes_p->racf_return_code == -1) {
    result_json["return_codes"]["racf_return_code"] = nullptr;
  }
  if (return_codes_p->racfu_return_code == -1) {
    result_json["return_codes"]["racfu_return_code"] = nullptr;
  }

  if (!errors.empty()) {
    result_json["errors"] = errors.error_messages;
  }

  if (intermediate_result_json != nullptr && errors.empty()) {
    if ((intermediate_result_json->empty()) && (operation == "extract")) {
      result_json["profile"] = nullptr;
    } else if (!intermediate_result_json->empty()) {
      result_json.merge_patch(*intermediate_result_json);
    }
  }

  // Convert profile JSON to C string.
  std::string result_json_cpp_string = result_json.dump();
  char *result_json_string           = static_cast<char *>(
      malloc(sizeof(char) * (result_json_cpp_string.size() + 1)));
  std::strcpy(result_json_string, result_json_cpp_string.c_str());

  // Build RACFu Result Structure
  racfu_result->raw_result         = raw_result;
  racfu_result->raw_result_length  = raw_result_length;
  racfu_result->raw_request        = raw_request;
  racfu_result->raw_request_length = raw_request_length;
  racfu_result->result_json        = result_json_string;

  logger_p->debug(MSG_DONE);

  return;
}
