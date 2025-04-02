#include "security_request.hpp"

#include "irrseq00.hpp"
#ifdef __TOS_390__
#include <unistd.h>
#else
#include "zoslib.h"
#endif

namespace RACFu {
SecurityRequest::SecurityRequest() { p_result_ = nullptr; }

SecurityRequest::SecurityRequest(racfu_result_t *p_result) {
  p_result_ = p_result;
  // Free dynamically allocated memory from previous requests.
  if (p_result->raw_request != nullptr) {
    Logger::getInstance().debugFree(p_result->raw_request, 64);
    free(p_result->raw_request);
    Logger::getInstance().debug("Done");
  }
  if (p_result->raw_result != nullptr) {
    Logger::getInstance().debugFree(p_result->raw_result, 31);
    free(p_result->raw_result);
    Logger::getInstance().debug("Done");
  }
  if (p_result->result_json != nullptr) {
    Logger::getInstance().debugFree(p_result->result_json, 64);
    free(p_result->result_json);
    Logger::getInstance().debug("Done");
  }
  p_result_->raw_request        = nullptr;
  p_result_->raw_request_length = 0;
  p_result_->raw_result         = nullptr;
  p_result_->raw_result_length  = 0;
  p_result_->result_json        = nullptr;
}

void SecurityRequest::load(const nlohmann::json &request) {
  admin_type_ = request["admin_type"].get<std::string>();
  operation_  = request["operation"].get<std::string>();

  if (request.contains("traits")) {
    traits_ = request["traits"].get<nlohmann::json>();
  }

  if (admin_type_ == "user") {
    function_code_ = USER_EXTRACT_FUNCTION_CODE;
    profile_name_  = request["userid"].get<std::string>();
  } else if (admin_type_ == "group") {
    function_code_ = GROUP_EXTRACT_FUNCTION_CODE;
    profile_name_  = request["group"].get<std::string>();
  } else if (admin_type_ == "group-connection") {
    function_code_ = GROUP_CONNECTION_EXTRACT_FUNCTION_CODE;
    if (operation_ == "extract") {
      profile_name_ = request["userid"].get<std::string>() + "." +
                      request["group"].get<std::string>();
    } else {
      profile_name_ = request["userid"].get<std::string>();
      group_        = request["group"].get<std::string>();
    }
  } else if (admin_type_ == "resource") {
    function_code_ = RESOURCE_EXTRACT_FUNCTION_CODE;
    profile_name_  = request["resource"].get<std::string>();
    class_name_    = request["class"].get<std::string>();
  } else if (admin_type_ == "data-set") {
    function_code_ = DATA_SET_EXTRACT_FUNCTION_CODE;
    profile_name_  = request["data_set"].get<std::string>();
  } else if (admin_type_ == "racf-options") {
    function_code_ = RACF_OPTIONS_EXTRACT_FUNCTION_CODE;
  } else if (admin_type_ == "permission") {
    if (request.contains("data_set")) {
      profile_name_ = request["data_set"].get<std::string>();
      class_name_   = "DATASET";
    } else {
      profile_name_ = request["resource"].get<std::string>();
      class_name_   = request["class"].get<std::string>();
    }
    if (request.contains("group")) {
      traits_["base:authid"] = request["group"].get<std::string>();
    } else {
      traits_["base:authid"] = request["userid"].get<std::string>();
    }
  }

  // set to 15 to enable precheck
  if (operation_ == "add") {
    irrsmo00_options_ = 15;
  } else if (operation_ == "alter") {
    if (admin_type_ != "group-connection" or admin_type_ != "racf-options" or
        admin_type_ != "permission") {
      irrsmo00_options_ = 15;
    }
  }

  if (request.contains("volume")) {
    volume_ = request["volume"].get<std::string>();
  }

  if (request.contains("generic")) {
    if (request["generic"].get<bool>() == true) {
      generic_ = "yes";
    } else {
      generic_ = "no";
    }
  }

  if (request.contains("run_as_userid")) {
    std::string run_as_userid_string = request.get<std::string>();
    Logger::getInstance().debug("Running under the authority of user: " +
                                run_as_userid_string);
    const int userid_length = run_as_userid_string.length();
    strncpy(run_as_userid_, run_as_userid_string.c_str(), userid_length);
    __a2e_l(run_as_userid_, userid_length);
  }
}

void SecurityRequest::buildResult() {
  Logger::getInstance().debug("Building result JSON ...");
  // Build Result JSON starting with Return Codes
  nlohmann::json result_json = {
      {"return_codes",
       {{"saf_return_code", return_codes_.saf_return_code},
        {"racf_return_code", return_codes_.racf_return_code},
        {"racf_reason_code", return_codes_.racf_reason_code},
        {"racfu_return_code", return_codes_.racfu_return_code}}}
  };

  // Convert '-1' to 'nullptr'
  if (return_codes_.saf_return_code == -1) {
    result_json["return_codes"]["saf_return_code"] = nullptr;
  }
  if (return_codes_.racf_return_code == -1) {
    result_json["return_codes"]["racf_return_code"] = nullptr;
  }
  if (return_codes_.racf_reason_code == -1) {
    result_json["return_codes"]["racf_reason_code"] = nullptr;
  }
  if (return_codes_.racf_return_code == -1) {
    result_json["return_codes"]["racf_return_code"] = nullptr;
  }
  if (return_codes_.racfu_return_code == -1) {
    result_json["return_codes"]["racfu_return_code"] = nullptr;
  }

  if (!errors_.empty()) {
    result_json["errors"] = errors_;
  }

  if (intermediate_result_json_ != nullptr && errors_.empty()) {
    if (!intermediate_result_json_.empty()) {
      result_json.merge_patch(intermediate_result_json_);
    }
  }

  // Convert profile JSON to C string.
  std::string result_json_string = result_json.dump();
  char *p_result_json            = static_cast<char *>(
      malloc(sizeof(char) * (result_json_string.size() + 1)));
  if (p_result_json == NULL) {
    perror("Warn - Unable to allocate space for the result JSON string.\n");
  } else {
    Logger::getInstance().debugAllocate(p_result_json, 64,
                                        result_json_string.size() + 1);
    std::strcpy(p_result_json, result_json_string.c_str());
  }

  // Save Result JSON
  p_result_->result_json = p_result_json;

  Logger::getInstance().debug("Done");
}
}  // namespace RACFu
