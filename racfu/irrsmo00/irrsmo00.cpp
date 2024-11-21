#include "irrsmo00.hpp"

#include <stdlib.h>

#include <cstring>

#include "errors.hpp"
#include "xml_generator.hpp"
#include "xml_parser.hpp"

#ifdef UNIT_TEST
#include "zoslib.h"
#else
#include <unistd.h>
#endif

char *call_irrsmo00(char *request_xml, char *running_userid,
                    unsigned int *result_buffer_size_p, int irrsmo00_options,
                    int *saf_rc_p, int *racf_rc_p, int *racf_rsn_p) {
  char work_area[1024];
  char req_handle[64] = {0};
  running_userid_t running_userid_struct = {
      (unsigned char)strlen(running_userid), {0}};
  unsigned int alet = 0;
  unsigned int acee = 0;
  char *result_buffer =
      static_cast<char *>(calloc(*result_buffer_size_p, sizeof(char)));
  int request_xml_length = strlen(request_xml);
  int result_len = *result_buffer_size_p;
  int num_parms = 17;
  int fn = 1;

  strncpy(running_userid_struct.running_userid, running_userid,
          running_userid_struct.running_userid_length);

  IRRSMO64(work_area, alet, saf_rc_p, alet, racf_rc_p, alet, racf_rsn_p,
           &num_parms, &fn, &irrsmo00_options, &request_xml_length, request_xml,
           req_handle, reinterpret_cast<char *>(&running_userid_struct), acee,
           &result_len, result_buffer);

  if (((*saf_rc_p != 8) || (*racf_rc_p != 4000)) ||
      ((*saf_rc_p == 8) && (*racf_rc_p == 4000) && (*racf_rsn_p > 100000000))) {
    *result_buffer_size_p = result_len;
    return result_buffer;
  }

  unsigned int new_result_buffer_size = *racf_rsn_p + result_len + 1;

  char *full_result =
      static_cast<char *>(calloc(new_result_buffer_size, sizeof(char)));
  char *result_buffer_ptr;
  strncpy(full_result, result_buffer, result_len);
  free(result_buffer);
  result_buffer_ptr = full_result + result_len * sizeof(unsigned char);
  *result_buffer_size_p = result_len;
  result_len = *racf_rsn_p;

  // Call IRRSMO64 Again with the appropriate buffer size
  IRRSMO64(work_area, alet, saf_rc_p, alet, racf_rc_p, alet, racf_rsn_p,
           &num_parms, &fn, &irrsmo00_options, &request_xml_length, request_xml,
           req_handle, reinterpret_cast<char *>(&running_userid_struct), acee,
           &result_len, result_buffer_ptr);

  *result_buffer_size_p += result_len;
  return full_result;
}

bool does_profile_exist(std::string admin_type, std::string profile_name,
                        const char *class_name, char *running_userid) {
  int irrsmo00_options, saf_rc = 0, racf_rc = 0, racf_rsn = 0;
  unsigned int result_buffer_size, request_length;
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

  irrsmo00_options = 13;
  result_buffer_size = 10000;

  // convert our c++ string to a char * buffer
  const int length = xml_buffer.length();
  char *request_buffer =
      static_cast<char *>(malloc(sizeof(char) * (length + 1)));
  strncpy(request_buffer, xml_buffer.c_str(), length + 1);
  __a2e_l(request_buffer, length);

  call_irrsmo00(request_buffer, running_userid, &result_buffer_size,
                irrsmo00_options, &saf_rc, &racf_rc, &racf_rsn);

  free(request_buffer);

  if ((racf_rc > 0) || (saf_rc > 0)) {
    return false;
  }
  return true;
}

int post_process_smo_json(nlohmann::json *results_p) {
  nlohmann::json commands = nlohmann::json::array();

  if (results_p->contains("error")) {
    // Only expected for irrsmo00 errors which are not expected, but possible
    std::string error_text;
    if ((*results_p)["error"].contains("textinerror")) {
      update_error_json(
          &(*results_p)["errors"], SMO_ERROR_WITH_TEXT,
          {
              {"error_message",
               (*results_p)["error"]["errormessage"].get<std::string>()},
              {"text_in_error",
               (*results_p)["error"]["textinerror"].get<std::string>() }
      });
      return 4;
    }
    update_error_json(
        &(*results_p)["errors"], SMO_ERROR_NO_TEXT,
        {
            {"error_message",
             (*results_p)["error"]["errormessage"].get<std::string>()}
    });
    results_p->erase("error");
    return 4;
  }

  if (!results_p->contains("command")) {
    // Only expected for "errors" cases
    return 4;
  }
  for (auto item = results_p->begin(); item != results_p->end();) {
    if ((item.key() == "info") || (item.key() == "command")) {
      item++;
    } else {
      item = results_p->erase(item);
    }
  }
  for (const auto &item : (*results_p)["command"].items()) {
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
  results_p->erase("command");
  (*results_p)["commands"] = commands;
  return 0;
}
