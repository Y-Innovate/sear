#include "irrsmo00.hpp"

#include <stdlib.h>

#include <cstring>

#include "xml_generator.hpp"
#include "xml_parser.hpp"

#ifdef __TOS_390__
#include <unistd.h>
#else
#include "zoslib.h"
#endif

char *call_irrsmo00(char *request_xml, const char *running_userid,
                    unsigned int &result_buffer_size, int irrsmo00_options,
                    racfu_return_codes_t &return_codes) {
  char work_area[1024];
  char req_handle[64]                    = {0};
  running_userid_t running_userid_struct = {
      (unsigned char)strlen(running_userid), {0}};
  unsigned int alet = 0;
  unsigned int acee = 0;
  char *result_buffer =
      static_cast<char *>(calloc(result_buffer_size, sizeof(char)));
  int request_xml_length = strlen(request_xml);
  int result_len         = result_buffer_size;
  int num_parms          = 17;
  int fn                 = 1;

  strncpy(running_userid_struct.running_userid, running_userid,
          running_userid_struct.running_userid_length);

  IRRSMO64(work_area, alet, &return_codes.saf_return_code, alet,
           &return_codes.racf_return_code, alet, &return_codes.racf_reason_code,
           &num_parms, &fn, &irrsmo00_options, &request_xml_length, request_xml,
           req_handle, reinterpret_cast<char *>(&running_userid_struct), acee,
           &result_len, result_buffer);

  // 'knownConditionTrueFalse' is a false positive, these conditionals work as
  // intended
  if (((return_codes.saf_return_code != 8) ||
       (return_codes.racf_return_code != 4000)) ||
      // cppcheck-suppress knownConditionTrueFalse
      ((return_codes.saf_return_code == 8) &&
       // cppcheck-suppress knownConditionTrueFalse
       (return_codes.racf_return_code == 4000) &&
       (return_codes.racf_reason_code > 100000000))) {
    result_buffer_size = result_len;
    return result_buffer;
  }

  unsigned int new_result_buffer_size =
      return_codes.racf_reason_code + result_len + 1;

  char *full_result =
      static_cast<char *>(calloc(new_result_buffer_size, sizeof(char)));
  char *result_buffer_ptr;
  strncpy(full_result, result_buffer, result_len);
  free(result_buffer);
  result_buffer_ptr  = full_result + result_len * sizeof(unsigned char);
  result_buffer_size = result_len;
  result_len         = return_codes.racf_reason_code;

  // Call IRRSMO64 Again with the appropriate buffer size
  IRRSMO64(work_area, alet, &return_codes.saf_return_code, alet,
           &return_codes.racf_return_code, alet, &return_codes.racf_reason_code,
           &num_parms, &fn, &irrsmo00_options, &request_xml_length, request_xml,
           req_handle, reinterpret_cast<char *>(&running_userid_struct), acee,
           &result_len, result_buffer_ptr);

  result_buffer_size += result_len;
  return full_result;
}

bool does_profile_exist(const std::string &admin_type,
                        const std::string &profile_name,
                        const std::string &class_name,
                        const char *running_userid) {
  std::string xml_buffer;

  if (admin_type == "resource") {
    xml_buffer =
        R"(<securityrequest xmlns="http://www.ibm.com/systems/zos/saf" xmlns:racf="http://www.ibm.com/systems/zos/racf"><)" +
        admin_type + R"( name=")" + profile_name + R"(" class=")" + class_name +
        R"("operation="listdata" requestid=")" + admin_type +
        R"(_request"/></securityrequest>)";
  } else {
    xml_buffer =
        R"(<securityrequest xmlns="http://www.ibm.com/systems/zos/saf" xmlns:racf="http://www.ibm.com/systems/zos/racf"><)" +
        admin_type + R"( name=")" + profile_name +
        R"(" operation="listdata" requestid=")" + admin_type +
        R"(_request"/></securityrequest>)";
  }

  int irrsmo00_options            = 13;
  unsigned int result_buffer_size = 10000;
  racfu_return_codes_t return_codes;

  // convert our c++ string to a char * buffer
  const int length = xml_buffer.length();
  char *request_buffer =
      static_cast<char *>(malloc(sizeof(char) * (length + 1)));
  strncpy(request_buffer, xml_buffer.c_str(), length + 1);
  __a2e_l(request_buffer, length);

  call_irrsmo00(request_buffer, running_userid, result_buffer_size,
                irrsmo00_options, return_codes);

  free(request_buffer);

  if ((return_codes.racf_return_code > 0) ||
      (return_codes.saf_return_code > 0)) {
    return false;
  }
  return true;
}

int post_process_smo_json(RACFu::Errors &errors, nlohmann::json &results,
                          const std::string &profile_name,
                          const std::string &admin_type,
                          const std::string &class_name) {
  nlohmann::json commands = nlohmann::json::array();

  if (results.contains("error")) {
    // Only expected for irrsmo00 errors which are not expected, but possible
    if (results["error"].contains("textinerror")) {
      errors.add_irrsmo00_error_message(
          results["error"]["errormessage"].get<std::string>() +
          " Text in error: " +
          results["error"]["textinerror"].get<std::string>());
      return 4;
    }
    errors.add_irrsmo00_error_message(
        results["error"]["errormessage"].get<std::string>());
    return 4;
  }

  if (results.contains("errors")) {
    // Only expected for "XML Parse Error"
    return 4;
  }

  if (!results.contains("command")) {
    // Only expected for "Add Protection" cases
    if (class_name.empty()) {
      errors.add_racfu_error_message("unable to add '" + profile_name +
                                     "' because a '" + admin_type +
                                     "' profile already exists with that name");
    } else {
      errors.add_racfu_error_message(
          "unable to add '" + profile_name + "' in the '" + class_name +
          "' class because a profile already exists with that name");
    }
    return 4;
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
  return 0;
}
