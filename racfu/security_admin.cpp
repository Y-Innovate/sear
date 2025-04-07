#include "security_admin.hpp"

#include <stdexcept>

#define _POSIX_C_SOURCE 200112L
#include <arpa/inet.h>

#include "irrsmo00.hpp"
#include "irrsmo00_error.hpp"
#include "profile_extractor.hpp"
#include "profile_post_processor.hpp"
#include "racfu_error.hpp"
#include "xml_generator.hpp"
#include "xml_parser.hpp"

namespace RACFu {
SecurityAdmin::SecurityAdmin(racfu_result_t *p_result, bool debug) {
  Logger::getInstance().setDebug(debug);
  request_ = SecurityRequest(p_result);
}

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

    Logger::getInstance().debug("Validating parameters ...");
    try {
      parameter_validator.validate(request_json);
    } catch (const std::exception &ex) {
      request_.return_codes_.racfu_return_code = 8;
      throw RACFuError(
          "The provided request JSON does not contain a valid request");
    }
    Logger::getInstance().debug("Done");

    // Load Request
    request_.load(request_json);

    // Make Request To Corresponding Callable Service
    if (request_.operation_ == "extract") {
      Logger::getInstance().debug("Entering IRRSEQ00 path");
      SecurityAdmin::doExtract();
    } else {
      Logger::getInstance().debug("Entering IRRSMO00 path");
      SecurityAdmin::doAddAlterDelete();
    }
  } catch (const RACFuError &ex) {
    request_.errors_ = ex.errors_;
  } catch (const IRRSMO00Error &ex) {
    request_.errors_ = ex.errors_;
  } catch (const std::exception &ex) {
    request_.errors_ = {ex.what()};
  }
  request_.buildResult();
}

void SecurityAdmin::doExtract() {
  // Extract Profile
  ProfileExtractor extractor;
  extractor.extract(request_);

  ProfilePostProcessor post_processor;
  if (request_.admin_type_ != "racf-options") {
    // Post Process Generic Extract Result
    post_processor.postProcessGeneric(request_);
  } else {
    // Post Process RACF Options Extract Result
    post_processor.postProcessRACFOptions(request_);
  }

  Logger::getInstance().debug("Profile extract result has been post-processed");
}

void SecurityAdmin::doAddAlterDelete() {
  IRRSMO00 irrsmo00;

  // Check if profile exists already for some alter operations
  if ((request_.operation_ == "alter") and
      ((request_.admin_type_ == "group") or (request_.admin_type_ == "user") or
       (request_.admin_type_ == "data-set") or
       (request_.admin_type_ == "resource"))) {
    Logger::getInstance().debug("Verifying that profile existis for alter ...");
    if (!irrsmo00.does_profile_exist(request_)) {
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
    std::free(request_.p_result_->raw_request);
    request_.p_result_->raw_request        = nullptr;
    request_.p_result_->raw_request_length = 0;
    std::free(request_.p_result_->raw_result);
    request_.p_result_->raw_result        = nullptr;
    request_.p_result_->raw_result_length = 0;

    Logger::getInstance().debug("Done");
  }

  // Build Request
  request_.p_result_->raw_result_length = 10000;
  XMLGenerator generator;
  generator.buildXMLString(request_);
  Logger::getInstance().debug("Calling IRRSMO00 ...");
  irrsmo00.call_irrsmo00(request_, false);
  Logger::getInstance().debug("Done");

  // Parse Result
  XMLParser parser;
  request_.intermediate_result_json_ = parser.buildJSONString(request_);
  Logger::getInstance().debug("Decoded Result:");
  Logger::getInstance().debug(request_.intermediate_result_json_.dump());

  // Post-Process Result
  irrsmo00.post_process_smo_json(request_, request_.intermediate_result_json_);

  Logger::getInstance().debug("Done");
}
}  // namespace RACFu
