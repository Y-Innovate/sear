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
  this->request = SecurityRequest(result);
  this->logger  = Logger(debug);
  this->errors  = Errors();
}

void SecurityAdmin::make_request(const char *request_json_string) {
  nlohmann::json request_json;

  // Parse Request JSON
  try {
    request_json = nlohmann::json::parse(request_json_string);
  } catch (const nlohmann::json::parse_error &ex) {
    this->errors.add_racfu_error_message(
        "Syntax error in request JSON at byte " + std::to_string(ex.byte));
    this->request.return_codes.racfu_return_code = 8;
    this->request.build_result({}, this->errors, this->logger);
    return;
  }

  this->logger.debug(MSG_VALIDATING_PARAMETERS);
  try {
    parameter_validator.validate(request_json);
  } catch (const std::exception &ex) {
    this->errors.add_racfu_error_message(
        "The provided request JSON does not contain a valid request");
    this->request.return_codes.racfu_return_code = 8;
    this->request.build_result({}, this->errors, this->logger);
    return;
  }
  this->logger.debug(MSG_DONE);

  this->request.load(request_json);
  // Make Request To Corresponding Callable Service
  if (this->request.operation == "extract") {
    this->logger.debug(MSG_SEQ_PATH);
    this->do_extract();
  } else {
    this->logger.debug(MSG_SMO_PATH);
    this->do_add_alter_delete();
  }
}

void SecurityAdmin::do_extract() {
  nlohmann::json profile_json;

  // Extract Profile
  extract(this->request, this->logger);
  if (this->request.result->raw_result == NULL) {
    this->request.return_codes.racfu_return_code = 4;
  } else {
    this->request.return_codes.racfu_return_code = 0;
  }

  // Build Failure Result
  if (this->request.result->raw_result == NULL) {
    if (this->request.admin_type != "racf-options") {
      this->errors.add_racfu_error_message(
          "unable to extract '" + this->request.admin_type + "' profile '" +
          this->request.profile_name + "'");
    } else {
      this->errors.add_racfu_error_message("unable to extract '" +
                                           this->request.admin_type + "'");
    }
    this->request.build_result(profile_json, this->errors, this->logger);
    return;
  }

  // Post Process Generic Result
  if (this->request.admin_type != "racf-options") {
    generic_extract_parms_results_t *generic_result_buffer =
        reinterpret_cast<generic_extract_parms_results_t *>(
            this->request.result->raw_result);
    this->request.result->raw_result_length =
        ntohl(generic_result_buffer->result_buffer_length);
    this->logger.debug(
        MSG_RESULT_SEQ_GENERIC,
        this->logger.cast_hex_string(this->request.result->raw_result,
                                     this->request.result->raw_result_length));
    profile_json =
        post_process_generic(generic_result_buffer, this->request.admin_type);
    // Post Process Setropts Result
  } else {
    setropts_extract_results_t *setropts_result_buffer =
        reinterpret_cast<setropts_extract_results_t *>(
            this->request.result->raw_result);
    this->request.result->raw_result_length =
        ntohl(setropts_result_buffer->result_buffer_length);
    this->logger.debug(
        MSG_RESULT_SEQ_SETROPTS,
        this->logger.cast_hex_string(this->request.result->raw_result,
                                     this->request.result->raw_result_length));
    profile_json = post_process_setropts(setropts_result_buffer);
  }

  this->logger.debug(MSG_SEQ_POST_PROCESS);

  // Build Success Result
  this->request.build_result(profile_json, this->errors, this->logger);
}

void SecurityAdmin::do_add_alter_delete() {
  // Check if profile exists already for some alter operations
  if ((this->request.operation == "alter") &&
      ((this->request.admin_type == "group") ||
       (this->request.admin_type == "user") ||
       (this->request.admin_type == "data-set") ||
       (this->request.admin_type == "resource"))) {
    this->logger.debug(MSG_SMO_VALIDATE_EXIST);
    if (!does_profile_exist(this->request, errors)) {
      if (this->request.class_name.empty()) {
        this->errors.add_racfu_error_message(
            "unable to alter '" + this->request.profile_name +
            "' because the profile does not exist");
      } else {
        this->errors.add_racfu_error_message(
            "Unable to alter '" + this->request.profile_name + "' in the '" +
            this->request.class_name +
            "' class because the profile does not exist");
      }
    }
    if (!this->errors.empty()) {
      this->request.return_codes.racfu_return_code = 8;
      this->request.build_result({}, this->errors, this->logger);
      return;
    }

    // Since the profile exists check was successful,
    // we can clean up the preserved result information.
    free(this->request.result->raw_request);
    this->request.result->raw_request        = nullptr;
    this->request.result->raw_request_length = 0;
    free(this->request.result->raw_result);
    this->request.result->raw_result        = nullptr;
    this->request.result->raw_result_length = 0;

    this->logger.debug(MSG_DONE);
  }

  // Build Request
  this->request.result->raw_result_length = 10000;
  XmlGenerator generator                  = XmlGenerator();

  generator.build_xml_string(this->request, this->errors, this->logger);

  if (!this->errors.empty()) {
    this->request.return_codes.racfu_return_code = 8;
    this->request.build_result({}, this->errors, this->logger);
    return;
  }

  this->logger.debug(MSG_CALLING_SMO);
  call_irrsmo00(this->request, this->errors, false);

  if (!this->errors.empty()) {
    this->request.return_codes.racfu_return_code = 8;
    this->request.build_result({}, this->errors, this->logger);
    return;
  }

  this->logger.debug(MSG_DONE);

  // Parse Result
  XmlParser parser = XmlParser();
  nlohmann::json intermediate_result_json =
      parser.build_json_string(this->request, this->errors, this->logger);

  if (!this->errors.empty()) {
    this->request.build_result({}, this->errors, this->logger);
    return;
  }

  this->logger.debug(MSG_SMO_POST_PROCESS);
  this->logger.debug(intermediate_result_json.dump());
  // Maintain any RC 4's from parsing xml or post-processing json
  this->request.return_codes.racfu_return_code =
      this->request.return_codes.racfu_return_code |
      post_process_smo_json(request, this->errors, intermediate_result_json);

  this->logger.debug(MSG_DONE);

  // Build Success Result
  this->request.build_result(intermediate_result_json, this->errors,
                             this->logger);
}
}  // namespace RACFu
