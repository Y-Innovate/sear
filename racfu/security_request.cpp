#include "security_request.hpp"

#include "irrseq00.hpp"
#include "messages.h"

#ifdef __TOS_390__
#include <unistd.h>
#else
#include "zoslib.h"
#endif

namespace RACFu {
SecurityRequest::SecurityRequest() { this->result = nullptr; }

SecurityRequest::SecurityRequest(racfu_result_t *result) {
  this->result                     = result;
  this->result->raw_request        = nullptr;
  this->result->raw_request_length = 0;
  this->result->raw_result         = nullptr;
  this->result->raw_result_length  = 0;
  this->result->result_json        = nullptr;
}

void SecurityRequest::load(const nlohmann::json &request) {
  this->admin_type = request["admin_type"].get<std::string>();
  this->operation  = request["operation"].get<std::string>();

  if (request.contains("traits")) {
    this->traits = request["traits"].get<nlohmann::json>();
  }

  if (this->admin_type == "user") {
    this->function_code = USER_EXTRACT_FUNCTION_CODE;
    this->profile_name  = request["userid"].get<std::string>();
  } else if (this->admin_type == "group") {
    this->function_code = GROUP_EXTRACT_FUNCTION_CODE;
    this->profile_name  = request["group"].get<std::string>();
  } else if (this->admin_type == "group-connection") {
    this->function_code = GROUP_CONNECTION_EXTRACT_FUNCTION_CODE;
    if (this->operation == "extract") {
      this->profile_name = request["userid"].get<std::string>() + "." +
                           request["group"].get<std::string>();
    } else {
      this->profile_name = request["userid"].get<std::string>();
      this->group        = request["group"].get<std::string>();
    }
  } else if (this->admin_type == "resource") {
    this->function_code = RESOURCE_EXTRACT_FUNCTION_CODE;
    this->profile_name  = request["resource"].get<std::string>();
    this->class_name    = request["class"].get<std::string>();
  } else if (this->admin_type == "data-set") {
    this->function_code = DATA_SET_EXTRACT_FUNCTION_CODE;
    this->profile_name  = request["data_set"].get<std::string>();
  } else if (this->admin_type == "racf-options") {
    this->function_code = SETROPTS_EXTRACT_FUNCTION_CODE;
  } else if (this->admin_type == "permission") {
    if (request.contains("data_set")) {
      this->profile_name = request["data_set"].get<std::string>();
      this->class_name   = "DATASET";
    } else {
      this->profile_name = request["resource"].get<std::string>();
      this->class_name   = request["class"].get<std::string>();
    }
    if (request.contains("group")) {
      this->traits["base:authid"] = request["group"].get<std::string>();
    } else {
      this->traits["base:authid"] = request["userid"].get<std::string>();
    }
  }

  // set to 15 to enable precheck
  if (this->operation == "add") {
    this->irrsmo00_options = 15;
  } else if (this->operation == "alter") {
    if (this->admin_type != "group-connection" or
        this->admin_type != "racf-options" or
        this->admin_type != "permission") {
      this->irrsmo00_options = 15;
    }
  }

  if (request.contains("volume")) {
    this->volume = request["volume"].get<std::string>();
  }

  if (request.contains("generic")) {
    if (request["generic"].get<bool>() == true) {
      this->generic = "yes";
    } else {
      this->generic = "no";
    }
  }

  if (request.contains("run_as_userid")) {
    std::string run_as_userid_string = request.get<std::string>();
    const int userid_length          = run_as_userid_string.length();
    strncpy(this->run_as_userid, run_as_userid_string.c_str(), userid_length);
    __a2e_l(this->run_as_userid, userid_length);
  }
}

void SecurityRequest::build_result(
    const nlohmann::json &intermediate_result_json, const Errors &errors,
    const Logger &logger) {
  logger.debug(MSG_BUILD_RESULT);
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

  if (!errors.empty()) {
    result_json["errors"] = errors.error_messages;
  }

  if (intermediate_result_json != nullptr && errors.empty()) {
    if (!intermediate_result_json.empty()) {
      result_json.merge_patch(intermediate_result_json);
    }
  }

  // Convert profile JSON to C string.
  std::string result_json_cpp_string = result_json.dump();
  char *result_json_string           = static_cast<char *>(
      malloc(sizeof(char) * (result_json_cpp_string.size() + 1)));
  if (result_json_string != NULL) {
    std::strcpy(result_json_string, result_json_cpp_string.c_str());
  }

  // Save Result JSON
  this->result->result_json = result_json_string;

  logger.debug(MSG_DONE);
}
}  // namespace RACFu
