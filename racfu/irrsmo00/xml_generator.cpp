#include "xml_generator.hpp"

#include <regex>

#include "key_map.hpp"
#include "logger.hpp"
#include "messages.h"
#include "trait_validation.hpp"

#ifdef __TOS_390__
#include <unistd.h>
#else
#include "zoslib.h"
#endif

// Public Functions of XmlGenerator
char* XmlGenerator::build_xml_string(
    const std::string& admin_type, nlohmann::json request,
    RACFu::Errors& errors, const std::string& profile_name,
    const std::string& auth_id, char* userid_buffer, int& irrsmo00_options,
    unsigned int& request_length, Logger& logger) {
  // Main body function that builds an xml string
  std::string true_admin_type, running_userid;
  // Build the securityrequest tag (Consistent)
  build_open_tag("securityrequest");
  build_attribute("xmlns", "http://www.ibm.com/systems/zos/saf");
  build_attribute("xmlns:racf", "http://www.ibm.com/systems/zos/racf");
  build_end_nested_tag();

  true_admin_type = convert_admin_type(admin_type);
  build_open_tag(true_admin_type);

  // The following options dictate parameters to IRRSMO00 and are not
  // built into XML
  if (request.contains("run_as_userid")) {
    running_userid = request["run_as_userid"].get<std::string>();
    request.erase("run_as_userid");
  }

  build_xml_header_attributes(true_admin_type, profile_name, request,
                              irrsmo00_options);

  if (!running_userid.empty()) {
    // Run this command as another user id
    logger.debug(MSG_RUN_AS_USER + running_userid);
    const int userid_length = running_userid.length();
    strncpy(userid_buffer, running_userid.c_str(), userid_length);
    __a2e_l(userid_buffer, userid_length);
  }

  build_attribute("requestid", true_admin_type + "_request");

  if (auth_id != "") {
    request["traits"]["base:authid"] = auth_id;
  }
  if (request.contains("traits") && !request["traits"].empty()) {
    build_end_nested_tag();

    logger.debug(MSG_VALIDATING_TRAITS);
    validate_traits(admin_type, request["traits"], errors);
    if (errors.empty()) {
      build_request_data(true_admin_type, admin_type, request["traits"]);
    } else {
      return nullptr;
    }
    logger.debug(MSG_DONE);

    // Close the admin object
    build_full_close_tag(true_admin_type);

    // Close the securityrequest tag (Consistent)
    build_full_close_tag("securityrequest");
  } else {
    // Close the admin object
    build_close_tag_no_value();
    // Close the securityrequest tag (Consistent)
    build_full_close_tag("securityrequest");
  }

  logger.debug(MSG_REQUEST_SMO_ASCII, xml_buffer);

  // convert our c++ string to a char * buffer
  const int length    = xml_buffer.length();
  char* output_buffer = static_cast<char*>(malloc(sizeof(char) * (length + 1)));
  strncpy(output_buffer, xml_buffer.c_str(), length + 1);
  __a2e_l(output_buffer, length);

  request_length = length;

  logger.debug(MSG_REQUEST_SMO_EBCDIC, logger.cast_hex_string(output_buffer));

  return output_buffer;
}

// Private Functions of XmlGenerator
std::string XmlGenerator::replace_xml_chars(std::string data) {
  // Replace xml-substituted characters with their substitution strings
  std::string amp = "&amp;", gt = "&gt;", lt = "&lt;", quot = "&quot;",
              apos = "&apos;";
  for (std::size_t i = 0; i < data.length(); i++) {
    if (data[i] == '&') {
      data.replace(i, 1, amp, 0, amp.length());
      i += amp.length() - 1 - 1;
    }
    if (data[i] == '<') {
      data.replace(i, 1, lt, 0, lt.length());
      i += lt.length() - 1 - 1;
    }
    if (data[i] == '>') {
      data.replace(i, 1, gt, 0, gt.length());
      i += gt.length() - 1 - 1;
    }
    if (data[i] == '"') {
      data.replace(i, 1, quot, 0, quot.length());
      i += quot.length() - 1 - 1;
    }
    if (data[i] == '\'') {
      data.replace(i, 1, apos, 0, apos.length());
      i += apos.length() - 1 - 1;
    }
  }
  return data;
}
void XmlGenerator::build_open_tag(std::string tag) {
  // Ex: "<base:universal_access"
  tag = replace_xml_chars(tag);
  xml_buffer.append("<" + tag);
}
void XmlGenerator::build_attribute(std::string name, std::string value) {
  // Ex: " operation=set"
  name  = replace_xml_chars(name);
  value = replace_xml_chars(value);
  xml_buffer.append(" " + name + "=\"" + value + "\"");
}
void XmlGenerator::build_value(std::string value) {
  // Ex: ">Read"
  value = replace_xml_chars(value);
  xml_buffer.append(">" + value);
}
void XmlGenerator::build_end_nested_tag() { xml_buffer.append(">"); }
void XmlGenerator::build_full_close_tag(std::string tag) {
  // Ex: "</base:universal_access>"
  tag = replace_xml_chars(tag);
  xml_buffer.append("</" + tag + ">");
}
void XmlGenerator::build_close_tag_no_value() { xml_buffer.append("/>"); }
void XmlGenerator::build_single_trait(const std::string& tag,
                                      const std::string& operation,
                                      const std::string& value) {
  // Combines above functions to build "trait" tags with added options and
  // values Ex: "<base:universal_access
  // operation=set>Read</base:universal_access>"
  build_open_tag(tag);
  if (operation.length() != 0) {
    build_attribute("operation", operation);
  }
  if (value.length() == 0) {
    build_close_tag_no_value();
  } else {
    build_value(value);
    build_full_close_tag(tag);
  }
}

void XmlGenerator::build_xml_header_attributes(
    const std::string& true_admin_type, const std::string& profile_name,
    const nlohmann::json& request, int& irrsmo00_options) {
  // Obtain JSON Header information and Build into Admin Object where
  // appropriate
  std::string operation = request["operation"].get<std::string>();
  if (operation == "add") {
    build_attribute("override", "no");
  }
  operation = convert_operation(operation, irrsmo00_options);
  build_attribute("operation", operation);
  if (request.contains("run")) {
    build_attribute("run", request["run"].get<std::string>());
  }
  if (true_admin_type == "systemsettings") {
    return;
  }
  build_attribute("name", profile_name);
  if ((true_admin_type == "user") || (true_admin_type == "group")) {
    return;
  }
  if (true_admin_type == "groupconnection") {
    build_attribute("group", request["group"].get<std::string>());
    return;
  }
  if ((true_admin_type == "resource") || (true_admin_type == "permission")) {
    std::string class_name = request["class"].get<std::string>();
    build_attribute("class", class_name);
    if (true_admin_type == "resource" || (class_name != "dataset")) {
      return;
    }
  }
  if ((true_admin_type == "dataset") || (true_admin_type == "permission")) {
    if (request.contains("volume")) {
      build_attribute("volume", request["volume"].get<std::string>());
    }
    if (request.contains("generic")) {
      if (request["generic"].get<bool>() == true) {
        build_attribute("generic", "yes");
      } else {
        build_attribute("generic", "no");
      }
    }
    return;
  }
  return;
}

void XmlGenerator::build_request_data(const std::string& true_admin_type,
                                      const std::string& admin_type,
                                      nlohmann::json request_data) {
  // Builds the xml for request data (segment-trait information) passed in a
  // json object
  nlohmann::json built_request{};
  std::string current_segment = "", item_segment, item_trait, item_operator;
  const char* translated_key;

  std::regex segment_trait_key_regex{R"~((([a-z]*):*)([a-z]*):(.*))~"};
  std::smatch segment_trait_key_data;

  auto item = request_data.begin();
  while (!request_data.empty()) {
    for (item = request_data.begin(); item != request_data.end();) {
      regex_match(item.key(), segment_trait_key_data, segment_trait_key_regex);
      if (segment_trait_key_data[3] == "") {
        item_operator = "";
        item_segment  = segment_trait_key_data[2];
      } else {
        item_operator = segment_trait_key_data[2];
        item_segment  = segment_trait_key_data[3];
      }
      item_trait = segment_trait_key_data[4];

      if (current_segment.empty()) {
        current_segment = item_segment;
        if ((true_admin_type != "systemsettings") &&
            (true_admin_type != "groupconnection") &&
            (true_admin_type != "permission")) {
          build_open_tag(current_segment);
          build_end_nested_tag();
        }
      }

      if ((item_segment.compare(current_segment) == 0)) {
        // Build each individual trait
        int8_t trait_operator = map_operator(item_operator);
        // Need to obtain the actual data
        int8_t trait_type = map_trait_type(item.value());
        translated_key = get_racf_key(admin_type.c_str(), item_segment.c_str(),
                                      (item_segment + ":" + item_trait).c_str(),
                                      trait_type, trait_operator);
        std::string trait_operator_str, value;
        switch (trait_type) {
          case TRAIT_TYPE_NULL:
            trait_operator_str = "del";
            value              = "";
            break;
          case TRAIT_TYPE_BOOLEAN:
            trait_operator_str = (item.value()) ? "set" : "del";
            value              = "";
            break;
          default:
            trait_operator_str = (item_operator.empty())
                                     ? "set"
                                     : convert_operator(item_operator);
            value              = (trait_type == TRAIT_TYPE_BOOLEAN)
                                     ? ""
                                     : json_value_to_string(item.value());
        }
        build_single_trait(("racf:" + std::string(translated_key)),
                           trait_operator_str, value);
        item = request_data.erase(item);

      } else
        item++;
    }
    if ((true_admin_type != "systemsettings") &&
        (true_admin_type != "groupconnection") &&
        (true_admin_type != "permission")) {
      build_full_close_tag(current_segment);
    }
    current_segment = "";
  }
}

std::string XmlGenerator::convert_operation(
    const std::string& request_operation, int& irrsmo00_options) {
  // Converts the designated function to the correct IRRSMO00 operation and
  // adjusts IRRSMO00 options as necessary (alter and add operations require
  // the PRECHECK attribute)
  if (request_operation == "add") {
    irrsmo00_options = 15;
    return "set";
  }
  if (request_operation == "alter") {
    irrsmo00_options = 15;
    return "set";
  }
  if (request_operation == "delete") {
    return "del";
  }
  if (request_operation == "extract") {
    return "listdata";
  }
  return "";
}

std::string XmlGenerator::convert_operator(const std::string& trait_operator) {
  // Converts the designated function to the correct IRRSMO00 operator
  if (trait_operator == "delete") {
    return "del";
  }
  return trait_operator;
}

std::string XmlGenerator::convert_admin_type(const std::string& admin_type) {
  // Converts the admin type between racfu's definitions and IRRSMO00's
  // definitions. group-connection to groupconnection, racf-options to
  // systemsettings and data-set to dataset. All other admin types should be
  // unchanged
  if (admin_type == "group-connection") {
    return "groupconnection";
  }
  if (admin_type == "racf-options") {
    return "systemsettings";
  }
  if (admin_type == "data-set") {
    return "dataset";
  }
  return admin_type;
}

std::string XmlGenerator::json_value_to_string(const nlohmann::json& trait) {
  if (trait.is_string()) {
    return trait.get<std::string>();
  }
  if (trait.is_array()) {
    std::string output_string = "";
    std::string delimeter =
        ", ";  // May just be " " or just be ","; May need to test
    for (const auto& item : trait.items()) {
      output_string += item.value().get<std::string>() + delimeter;
    }
    for (int i = 0; i < delimeter.length(); i++) {
      output_string.pop_back();
    }
    return output_string;
  }
  return trait.dump();
}
