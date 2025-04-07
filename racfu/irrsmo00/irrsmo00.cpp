#include "irrsmo00.hpp"

#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

#include "irrsmo00_error.hpp"
#include "racfu_error.hpp"
#include "xml_generator.hpp"
#include "xml_parser.hpp"

#ifdef __TOS_390__
#include <unistd.h>
#else
#include "zoslib.h"
#endif

namespace RACFu {
void IRRSMO00::call_irrsmo00(SecurityRequest &request,
                             bool profile_exists_check) {
  char work_area[1024];
  char req_handle[64]                    = {0};
  running_userid_t running_userid_struct = {
      (unsigned char)std::strlen(request.run_as_userid_), {0}};
  unsigned int alet    = 0;
  unsigned int acee    = 0;
  int num_parms        = 17;
  int fn               = 1;

  int irrsmo00_options = request.irrsmo00_options_;
  if (profile_exists_check == true) {
    irrsmo00_options = 13;
  }

  std::strncpy(running_userid_struct.running_userid, request.run_as_userid_,
               running_userid_struct.running_userid_length);

  auto result_unique_ptr =
      std::make_unique<char[]>(request.p_result_->raw_result_length);
  std::memset(result_unique_ptr.get(), 0, request.p_result_->raw_result_length);

  IRRSMO64(work_area, alet, &request.return_codes_.saf_return_code, alet,
           &request.return_codes_.racf_return_code, alet,
           &request.return_codes_.racf_reason_code, &num_parms, &fn,
           &irrsmo00_options, &request.p_result_->raw_request_length,
           request.p_result_->raw_request, req_handle,
           reinterpret_cast<char *>(&running_userid_struct), acee,
           &request.p_result_->raw_result_length, result_unique_ptr.get());

  // 'knownConditionTrueFalse' is a false positive. These conditionals work as
  // intended
  if (((request.return_codes_.saf_return_code != 8) or
       (request.return_codes_.racf_return_code != 4000)) or
      // cppcheck-suppress knownConditionTrueFalse
      ((request.return_codes_.saf_return_code == 8) and
       // cppcheck-suppress knownConditionTrueFalse
       (request.return_codes_.racf_return_code == 4000) and
       (request.return_codes_.racf_reason_code > 100000000))) {
    request.p_result_->raw_result = result_unique_ptr.get();
    result_unique_ptr.release();
    return;
  }

  // Handle result buffer too small scenario.
  int bytes_remaining = request.return_codes_.racf_reason_code;
  int new_result_length =
      request.p_result_->raw_result_length + bytes_remaining + 1;
  auto full_result_unique_ptr = std::make_unique<char[]>(new_result_length);
  std::memset(full_result_unique_ptr.get(), 0, new_result_length);
  std::memcpy(full_result_unique_ptr.get(), result_unique_ptr.get(),
              request.p_result_->raw_result_length);

  char *p_next_byte =
      full_result_unique_ptr.get() +
      request.p_result_->raw_result_length * sizeof(unsigned char);

  IRRSMO64(work_area, alet, &request.return_codes_.saf_return_code, alet,
           &request.return_codes_.racf_return_code, alet,
           &request.return_codes_.racf_reason_code, &num_parms, &fn,
           &irrsmo00_options, &request.p_result_->raw_request_length,
           request.p_result_->raw_request, req_handle,
           reinterpret_cast<char *>(&running_userid_struct), acee,
           &bytes_remaining, p_next_byte);

  request.p_result_->raw_result_length = new_result_length;
  request.p_result_->raw_result        = full_result_unique_ptr.get();
  full_result_unique_ptr.release();
}

bool IRRSMO00::does_profile_exist(SecurityRequest &request) {
  std::string xml_string;

  if (request.admin_type_ == "resource") {
    Logger::getInstance().debug("Checking if '" + request.admin_type_ +
                                "' profile '" + request.profile_name_ +
                                "' already exists in the '" +
                                request.class_name_ + "' ...");
    xml_string =
        R"(<securityrequest xmlns="http://www.ibm.com/systems/zos/saf" xmlns:racf="http://www.ibm.com/systems/zos/racf"><)" +
        request.admin_type_ + R"( name=")" + request.profile_name_ +
        R"(" class=")" + request.class_name_ +
        R"("operation="listdata" requestid=")" + request.admin_type_ +
        R"(_request"/></securityrequest>)";
  } else {
    Logger::getInstance().debug("Checking if '" + request.admin_type_ +
                                "' profile '" + request.profile_name_ +
                                "' already exists ...");
    xml_string =
        R"(<securityrequest xmlns="http://www.ibm.com/systems/zos/saf" xmlns:racf="http://www.ibm.com/systems/zos/racf"><)" +
        request.admin_type_ + R"( name=")" + request.profile_name_ +
        R"(" operation="listdata" requestid=")" + request.admin_type_ +
        R"(_request"/></securityrequest>)";
  }

  request.p_result_->raw_result_length = 10000;

  // convert our c++ string to a char * buffer
  auto request_unique_ptr = std::make_unique<char[]>(xml_string.length());
  Logger::getInstance().debugAllocate(request_unique_ptr.get(), 64,
                                      xml_string.length());
  request.p_result_->raw_request_length = xml_string.length();
  request.p_result_->raw_request        = request_unique_ptr.get();
  request_unique_ptr.release();
  std::strncpy(request.p_result_->raw_request, xml_string.c_str(),
               request.p_result_->raw_request_length);
  __a2e_l(request.p_result_->raw_request,
          request.p_result_->raw_request_length);

  IRRSMO00::call_irrsmo00(request, true);

  Logger::getInstance().debug("Done");

  if (request.p_result_->raw_result == nullptr) {
    return false;
  }

  if ((request.return_codes_.racf_return_code > 0) or
      (request.return_codes_.saf_return_code > 0)) {
    return false;
  }

  return true;
}

void IRRSMO00::post_process_smo_json(SecurityRequest &request,
                                     nlohmann::json &results) {
  nlohmann::json commands = nlohmann::json::array();

  if (results.contains("error")) {
    request.return_codes_.racfu_return_code = 4;
    // Only expected for irrsmo00 errors which are not expected, but possible
    if (results["error"].contains("textinerror")) {
      throw IRRSMO00Error(results["error"]["errormessage"].get<std::string>() +
                          " Text in error: " +
                          results["error"]["textinerror"].get<std::string>());
    }
    throw IRRSMO00Error(results["error"]["errormessage"].get<std::string>());
  }

  if (results.contains("errors")) {
    // Only expected for "XML Parse Error"
    request.return_codes_.racfu_return_code = 4;
    throw IRRSMO00Error(results["errors"].get<std::vector<std::string>>());
  }

  if (!results.contains("command")) {
    // Only expected for "Add Protection" cases
    request.return_codes_.racfu_return_code = 4;
    if (request.class_name_.empty()) {
      throw RACFuError("unable to add '" + request.profile_name_ +
                       "' because a '" + request.admin_type_ +
                       "' profile already exists with that name");
    } else {
      throw RACFuError(
          "unable to add '" + request.profile_name_ + "' in the '" +
          request.class_name_ +
          "' class because a profile already exists with that name");
    }
  }

  for (auto item = results.begin(); item != results.end();) {
    if ((item.key() == "command")) {
      item++;
    } else {
      item = results.erase(item);
    }
  }

  if (results["command"].contains("image")) {
    // If there is only one command in the json
    nlohmann::json command;
    command["command"]  = results["command"]["image"];
    command["messages"] = nlohmann::json::array();
    if (results["command"].contains("message")) {
      if (results["command"]["message"].is_array()) {
        command["messages"].merge_patch(results["command"]["message"]);
      } else {
        command["messages"].push_back(results["command"]["message"]);
      }
    }
    commands.push_back(command);
  } else {
    // Iterate through a list of commands
    for (const auto &item : results["command"].items()) {
      nlohmann::json current_command{};
      if (item.value().contains("image")) {
        current_command["command"] = item.value()["image"];
      }
      current_command["messages"] = nlohmann::json::array();
      if (item.value().contains("message")) {
        if (item.value()["message"].is_array()) {
          current_command["messages"].merge_patch(item.value()["message"]);
        } else {
          current_command["messages"].push_back(item.value()["message"]);
        }
      }
      commands.push_back(current_command);
    }
  }
  results.erase("command");
  results["commands"] = commands;
}
}  // namespace RACFu
