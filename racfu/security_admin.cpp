#include "security_admin.hpp"

#include <stdexcept>

#define _POSIX_C_SOURCE 200112L
#include <arpa/inet.h>

#include "extract.hpp"
#include "irrsmo00.hpp"
#include "irrsmo00_error.hpp"
#include "messages.h"
#include "post_process.hpp"
#include "racfu_error.hpp"
#include "xml_generator.hpp"
#include "xml_parser.hpp"

namespace RACFu {
SecurityAdmin::SecurityAdmin(racfu_result_t *p_result, bool debug)
    : request_(SecurityRequest(p_result)), logger_(Logger(debug)) {}

void SecurityAdmin::makeRequest(const char *p_request_json_string) {
  nlohmann::json request_json;

  try {
    // Parse Request JSON
    try {
      request_json = nlohmann::json::parse(p_request_json_string);
    } catch (const nlohmann::json::parse_error &ex) {
      request_.return_codes_.racfu_return_code = 8;
      throw RACFuError(std::string("Syntax error in request JSON at byte ") +
                       std::to_string(ex.byte));
    }

    logger_.debug(MSG_VALIDATING_PARAMETERS);
    try {
      parameter_validator.validate(request_json);
    } catch (const std::exception &ex) {
      request_.return_codes_.racfu_return_code = 8;
      throw RACFuError(
          "The provided request JSON does not contain a valid request");
    }
    logger_.debug(MSG_DONE);

    // Load Request
    request_.load(request_json);

    // Make Request To Corresponding Callable Service
    if (request_.operation_ == "extract") {
      logger_.debug(MSG_SEQ_PATH);
      this->doExtract();
    } else {
      logger_.debug(MSG_SMO_PATH);
      this->doAddAlterDelete();
    }
  } catch (const RACFuError &ex) {
    request_.errors_ = ex.errors_;
  } catch (const IRRSMO00Error &ex) {
    request_.errors_ = ex.errors_;
  } catch (const std::exception &ex) {
    request_.errors_ = {ex.what()};
  }
  request_.buildResult(logger_);
}

void SecurityAdmin::doExtract() {
  // Extract Profile
  extract(request_, logger_);
  if (request_.p_result_->raw_result == NULL) {
    request_.return_codes_.racfu_return_code = 4;
    // Raise Exception if Extract Failed.
    if (request_.admin_type_ != "racf-options") {
      throw RACFuError("unable to extract '" + request_.admin_type_ +
                       "' profile '" + request_.profile_name_ + "'");
    } else {
      throw RACFuError("unable to extract '" + request_.admin_type_ + "'");
    }
  } else {
    request_.return_codes_.racfu_return_code = 0;
  }

  // Post Process Generic Result
  if (request_.admin_type_ != "racf-options") {
    generic_extract_parms_results_t *p_generic_result =
        reinterpret_cast<generic_extract_parms_results_t *>(
            request_.p_result_->raw_result);
    request_.p_result_->raw_result_length =
        ntohl(p_generic_result->result_buffer_length);
    logger_.debug(
        MSG_RESULT_SEQ_GENERIC,
        logger_.cast_hex_string(request_.p_result_->raw_result,
                                request_.p_result_->raw_result_length));
    request_.intermediate_result_json_ =
        post_process_generic(p_generic_result, request_.admin_type_);
    // Post Process Setropts Result
  } else {
    setropts_extract_results_t *p_setropts_result =
        reinterpret_cast<setropts_extract_results_t *>(
            request_.p_result_->raw_result);
    request_.p_result_->raw_result_length =
        ntohl(p_setropts_result->result_buffer_length);
    logger_.debug(
        MSG_RESULT_SEQ_SETROPTS,
        logger_.cast_hex_string(request_.p_result_->raw_result,
                                request_.p_result_->raw_result_length));
    request_.intermediate_result_json_ =
        post_process_setropts(p_setropts_result);
  }

  logger_.debug(MSG_SEQ_POST_PROCESS);
}

void SecurityAdmin::doAddAlterDelete() {
  // Check if profile exists already for some alter operations
  if ((request_.operation_ == "alter") &&
      ((request_.admin_type_ == "group") || (request_.admin_type_ == "user") ||
       (request_.admin_type_ == "data-set") ||
       (request_.admin_type_ == "resource"))) {
    logger_.debug(MSG_SMO_VALIDATE_EXIST);
    if (!does_profile_exist(request_)) {
      request_.return_codes_.racfu_return_code = 8;
      if (request_.class_name_.empty()) {
        throw RACFuError("unable to alter '" + request_.profile_name_ +
                         "' because the profile does not exist");
      } else {
        throw RACFuError("unable to alter '" + request_.profile_name_ +
                         "' in the '" + request_.class_name_ +
                         "' class because the profile does not exist");
      }
    }

    // Since the profile exists check was successful,
    // we can clean up the preserved result information.
    free(request_.p_result_->raw_request);
    request_.p_result_->raw_request        = nullptr;
    request_.p_result_->raw_request_length = 0;
    free(request_.p_result_->raw_result);
    request_.p_result_->raw_result        = nullptr;
    request_.p_result_->raw_result_length = 0;

    logger_.debug(MSG_DONE);
  }

  // Build Request
  request_.p_result_->raw_result_length = 10000;
  XmlGenerator generator                = XmlGenerator();

  generator.build_xml_string(request_, logger_);

  logger_.debug(MSG_CALLING_SMO);
  call_irrsmo00(request_, false);

  logger_.debug(MSG_DONE);

  // Parse Result
  XmlParser parser = XmlParser();
  request_.intermediate_result_json_ =
      parser.build_json_string(request_, logger_);

  logger_.debug(MSG_SMO_POST_PROCESS);
  logger_.debug(request_.intermediate_result_json_.dump());

  // Post-Process Result
  post_process_smo_json(request_, request_.intermediate_result_json_);

  logger_.debug(MSG_DONE);
}
}  // namespace RACFu
