#include "irrsmo00.hpp"

#include <stdlib.h>

#include <cstring>
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

void call_irrsmo00(RACFu::SecurityRequest &request, bool profile_exists_check) {
  char work_area[1024];
  char req_handle[64]                    = {0};
  running_userid_t running_userid_struct = {
      (unsigned char)strlen(request.run_as_userid_), {0}};
  unsigned int alet             = 0;
  unsigned int acee             = 0;
  request.p_result_->raw_result = static_cast<char *>(
      calloc(request.p_result_->raw_result_length, sizeof(char)));
  if (request.p_result_->raw_result == NULL) {
    request.return_codes_.racfu_return_code = 8;
    throw RACFu::RACFuError(
        "Unable to allocate a result buffer for the IRRSMO00 result");
  }
  int num_parms        = 17;
  int fn               = 1;

  int irrsmo00_options = request.irrsmo00_options_;
  if (profile_exists_check == true) {
    irrsmo00_options = 13;
  }

  strncpy(running_userid_struct.running_userid, request.run_as_userid_,
          running_userid_struct.running_userid_length);

  IRRSMO64(work_area, alet, &request.return_codes_.saf_return_code, alet,
           &request.return_codes_.racf_return_code, alet,
           &request.return_codes_.racf_reason_code, &num_parms, &fn,
           &irrsmo00_options, &request.p_result_->raw_request_length,
           request.p_result_->raw_request, req_handle,
           reinterpret_cast<char *>(&running_userid_struct), acee,
           &request.p_result_->raw_result_length,
           request.p_result_->raw_result);

  // 'knownConditionTrueFalse' is a false positive. These conditionals work as
  // intended
  if (((request.return_codes_.saf_return_code != 8) ||
       (request.return_codes_.racf_return_code != 4000)) ||
      // cppcheck-suppress knownConditionTrueFalse
      ((request.return_codes_.saf_return_code == 8) &&
       // cppcheck-suppress knownConditionTrueFalse
       (request.return_codes_.racf_return_code == 4000) &&
       (request.return_codes_.racf_reason_code > 100000000))) {
    return;
  }

  /*
  unsigned int new_result_buffer_size =
      request.return_codes.racf_reason_code + result_len + 1;

  char *full_result =
      static_cast<char *>(calloc(new_result_buffer_size, sizeof(char)));
  if (full_result == NULL) {
    errors.add_racfu_error_message("Allocation of 'full_result' failed");
    return NULL;
  }
  char *result_buffer_ptr;
  strncpy(full_result, result_buffer, result_len);
  free(result_buffer);
  result_buffer_ptr  = full_result + result_len * sizeof(unsigned char);
  result_buffer_size = result_len;
  result_len         = request.return_codes.racf_reason_code;

  // Call IRRSMO64 Again with the appropriate buffer size
  IRRSMO64(work_area, alet, &request.return_codes.saf_return_code, alet,
           &request.return_codes.racf_return_code, alet,
  &request.return_codes.racf_reason_code, &num_parms, &fn,
  &request.irrsmo00_options, &request_xml_length, request_xml, req_handle,
  reinterpret_cast<char *>(&running_userid_struct), acee, &result_len,
  result_buffer_ptr);

  result_buffer_size += result_len;
  return full_result;
  */
}

bool does_profile_exist(RACFu::SecurityRequest &request) {
  std::string xml_buffer;

  if (request.admin_type_ == "resource") {
    xml_buffer =
        R"(<securityrequest xmlns="http://www.ibm.com/systems/zos/saf" xmlns:racf="http://www.ibm.com/systems/zos/racf"><)" +
        request.admin_type_ + R"( name=")" + request.profile_name_ +
        R"(" class=")" + request.class_name_ +
        R"("operation="listdata" requestid=")" + request.admin_type_ +
        R"(_request"/></securityrequest>)";
  } else {
    xml_buffer =
        R"(<securityrequest xmlns="http://www.ibm.com/systems/zos/saf" xmlns:racf="http://www.ibm.com/systems/zos/racf"><)" +
        request.admin_type_ + R"( name=")" + request.profile_name_ +
        R"(" operation="listdata" requestid=")" + request.admin_type_ +
        R"(_request"/></securityrequest>)";
  }

  request.p_result_->raw_result_length = 10000;

  // convert our c++ string to a char * buffer
  request.p_result_->raw_request_length = xml_buffer.length();
  request.p_result_->raw_request        = static_cast<char *>(
      malloc(sizeof(char) * (request.p_result_->raw_request_length + 1)));
  if (request.p_result_->raw_request == NULL) {
    request.return_codes_.racfu_return_code = 8;
    throw RACFu::RACFuError(
        "Unable to allocate a request buffer for profile check request");
  }
  strncpy(request.p_result_->raw_request, xml_buffer.c_str(),
          request.p_result_->raw_request_length);
  __a2e_l(request.p_result_->raw_request,
          request.p_result_->raw_request_length);

  call_irrsmo00(request, true);

  if (request.p_result_->raw_result == NULL) {
    return false;
  }

  if ((request.return_codes_.racf_return_code > 0) ||
      (request.return_codes_.saf_return_code > 0)) {
    return false;
  }

  return true;
}

void post_process_smo_json(RACFu::SecurityRequest &request,
                           nlohmann::json &results) {
  nlohmann::json commands = nlohmann::json::array();

  if (results.contains("error")) {
    request.return_codes_.racfu_return_code = 4;
    // Only expected for irrsmo00 errors which are not expected, but possible
    if (results["error"].contains("textinerror")) {
      throw RACFu::IRRSMO00Error(
          results["error"]["errormessage"].get<std::string>() +
          " Text in error: " +
          results["error"]["textinerror"].get<std::string>());
    }
    throw RACFu::IRRSMO00Error(
        results["error"]["errormessage"].get<std::string>());
  }

  if (results.contains("errors")) {
    // Only expected for "XML Parse Error"
    request.return_codes_.racfu_return_code = 4;
    throw RACFu::IRRSMO00Error(
        results["errors"].get<std::vector<std::string>>());
  }

  if (!results.contains("command")) {
    // Only expected for "Add Protection" cases
    request.return_codes_.racfu_return_code = 4;
    if (request.class_name_.empty()) {
      throw RACFu::RACFuError("unable to add '" + request.profile_name_ +
                              "' because a '" + request.admin_type_ +
                              "' profile already exists with that name");
    } else {
      throw RACFu::RACFuError(
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
