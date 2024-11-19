#include "xml_generator.hpp"

#include <regex>
#include <string>

#include "key_map.hpp"
#include "logger.hpp"
#include "messages.h"
#include "trait_validation.hpp"

#ifdef UNIT_TEST
#include "zoslib.h"
#else
#include <unistd.h>
#endif

// Public Functions of XmlGenerator
char* XmlGenerator::build_xml_string(const char* admin_type,
                                     nlohmann::json* request,
                                     nlohmann::json* errors,
                                     char* userid_buffer, int* irrsmo00_options,
                                     unsigned int* request_length,
                                     Logger* logger_p) {
  // Main body function that builds an xml string
  std::string adminType, runningUserId;
  nlohmann::json requestData;

  // Build the securityrequest tag (Consistent)
  build_open_tag("securityrequest");
  build_attribute("xmlns", "http://www.ibm.com/systems/zos/saf");
  build_attribute("xmlns:racf", "http://www.ibm.com/systems/zos/racf");
  build_end_nested_tag();

  adminType = convert_admin_type(std::string(admin_type));
  build_open_tag(adminType);

  // The following options dictate parameters to IRRSMO00 and are not
  // built into XML
  if (request->contains("run_as_user_id")) {
    runningUserId = (*request)["run_as_user_id"].get<std::string>();
    request->erase("run_as_user_id");
  }

  build_xml_header_attributes(adminType, request, irrsmo00_options);

  if (!runningUserId.empty()) {
    // Run this command as another user id
    logger_p->debug(MSG_RUN_AS_USER + runningUserId);
    const int userid_length = runningUserId.length();
    strncpy(userid_buffer, runningUserId.c_str(), userid_length);
    __a2e_l(userid_buffer, userid_length);
  }

  build_attribute("requestid", adminType + "_request");

  if ((request->contains("traits")) && (!(*request)["traits"].empty())) {
    build_end_nested_tag();

    logger_p->debug(MSG_VALIDATING_TRAITS);
    validate_traits(admin_type, &((*request)["traits"]), errors);
    if (errors->empty()) {
      build_request_data(adminType, &((*request)["traits"]));
    } else {
      return nullptr;
    }
    logger_p->debug(MSG_DONE);

    // Close the admin object
    build_full_close_tag(adminType);

    // Close the securityrequest tag (Consistent)
    build_full_close_tag("securityrequest");
  } else {
    // Close the admin object
    build_close_tag_no_value();
  }

  logger_p->debug(MSG_REQUEST_SMO_ASCII, xml_buffer);

  // convert our c++ string to a char * buffer
  const int length = xml_buffer.length();
  char* output_buffer = static_cast<char*>(malloc(sizeof(char) * (length + 1)));
  strncpy(output_buffer, xml_buffer.c_str(), length + 1);
  __a2e_l(output_buffer, length);

  *request_length = length;

  logger_p->debug(MSG_REQUEST_SMO_EBCDIC,
                  logger_p->cast_hex_string(output_buffer));

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
  name = replace_xml_chars(name);
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
void XmlGenerator::build_single_trait(std::string tag, std::string operation,
                                      std::string value) {
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

void XmlGenerator::build_xml_header_attributes(std::string adminType,
                                               nlohmann::json* request,
                                               int* irrsmo00_options) {
  // Obtain JSON Header information and Build into Admin Object where
  // appropriate

  std::string className, operation;

  operation = (*request)["operation"].get<std::string>();
  if (operation == "add") {
    build_attribute("override", "no");
  }
  operation = convert_operation(operation, irrsmo00_options);
  build_attribute("operation", operation);
  if (request->contains("run")) {
    build_attribute("run", (*request)["run"].get<std::string>());
  }
  if (adminType == "systemsettings") {
    return;
  }
  build_attribute("name", (*request)["profile_name"].get<std::string>());
  if ((adminType == "user") || (adminType == "group")) {
    return;
  }
  if (adminType == "groupconnection") {
    build_attribute("group", (*request)["group"].get<std::string>());
    return;
  }
  if ((adminType == "resource") || (adminType == "permission")) {
    className = (*request)["class"].get<std::string>();
    build_attribute("class", className);
    if (adminType == "resource" || (className != "dataset")) {
      return;
    }
  }
  if ((adminType == "dataset") || (adminType == "permission")) {
    if (request->contains("volume")) {
      build_attribute("volume", (*request)["volume"].get<std::string>());
    }
    if (request->contains("generic")) {
      build_attribute("generic", (*request)["generic"].get<std::string>());
    }
    return;
  }
  return;
}

nlohmann::json XmlGenerator::build_request_data(std::string adminType,
                                                nlohmann::json* requestData) {
  // Builds the xml for request data (segment-trait information) passed in a
  // json object
  nlohmann::json errors;
  std::string currentSegment = "", itemSegment, itemTrait, itemOperation;
  const char* translatedKey;

  std::regex segment_trait_key_regex{R"~((([a-z]*):*)([a-z]*):(.*))~"};
  std::smatch segment_trait_key_data;

  auto item = requestData->begin();
  while (!requestData->empty()) {
    for (item = requestData->begin(); item != requestData->end();) {
      regex_match(item.key(), segment_trait_key_data, segment_trait_key_regex);
      if (segment_trait_key_data[3] == "") {
        itemOperation = "";
        itemSegment = segment_trait_key_data[2];
      } else {
        itemOperation = segment_trait_key_data[2];
        itemSegment = segment_trait_key_data[3];
      }
      itemTrait = segment_trait_key_data[4];

      if (currentSegment.empty()) {
        currentSegment = itemSegment;
        build_open_tag(currentSegment);
        build_end_nested_tag();
      }

      if ((itemSegment.compare(currentSegment) == 0)) {
        // Build each individual trait
        int8_t operation = map_operation(itemOperation);
        // Need to obtain the actual data
        int8_t trait_type = map_trait_type(item.value());
        translatedKey = get_racf_key(adminType.c_str(), itemSegment.c_str(),
                                     (itemSegment + ":" + itemTrait).c_str(),
                                     trait_type, operation);
        std::string operation_str =
            (itemOperation.empty()) ? "set" : itemOperation;
        std::string value = (item.value().is_boolean())
                                ? ""
                                : json_value_to_string(item.value());
        build_single_trait(("racf:" + std::string(translatedKey)),
                           operation_str, value);
        item = requestData->erase(item);

      } else
        item++;
    }
    build_full_close_tag(currentSegment);
    currentSegment = "";
  }
  return errors;
}

std::string XmlGenerator::convert_operation(std::string requestOperation,
                                            int* irrsmo00_options) {
  // Converts the designated function to the correct IRRSMO00 operation and
  // adjusts IRRSMO00 options as necessary (alter and add operations require
  // the PRECHECK attribute)
  if (requestOperation == "add") {
    *irrsmo00_options = 15;
    return "set";
  }
  if (requestOperation == "alter") {
    *irrsmo00_options = 15;
    return "set";
  }
  if (requestOperation == "delete") {
    return "del";
  }
  if (requestOperation == "extract") {
    return "listdata";
  }
  return "";
}

std::string XmlGenerator::convert_admin_type(std::string admin_type) {
  // Converts the admin type between racfu's definitions and IRRSMO00's
  // definitions. group-connection to groupconnection, setropts to
  // systemsettings and data-set to dataset. All other admin types should be
  // unchanged
  if (admin_type == "group-connection") {
    return "groupconnection";
  }
  if (admin_type == "setropts") {
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
