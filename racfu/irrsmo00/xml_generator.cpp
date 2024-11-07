#include "xml_generator.hpp"

#include <unistd.h>

#include <iostream>
#include <regex>
#include <string>

#include "key_map.hpp"
#include "parameter_validation.hpp"

// Public Functions of XmlGenerator
char* XmlGenerator::build_xml_string(const char* admin_type,
                                     nlohmann::json request,
                                     nlohmann::json* errors,
                                     char* userid_buffer, int* irrsmo00_options,
                                     unsigned int* result_buffer_size,
                                     unsigned int* request_length,
                                     bool* debug) {
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
  if (request.contains("running_user_id")) {
    runningUserId = request["running_user_id"].get<std::string>();
    request.erase("running_user_id");
  }
  if (request.contains("result_buffer_size")) {
    *result_buffer_size = request["result_buffer_size"].get<uint>();
    request.erase("result_buffer_size");
  }
  if (request.contains("debug_mode")) {
    *debug = request["debug_mode"].get<bool>();
    request.erase("debug_mode");
  }

  build_xml_head_attributes(adminType, request, irrsmo00_options);

  if (!runningUserId.empty()) {
    // Run this command as another user id
    const int userid_length = runningUserId.length();
    strncpy(userid_buffer, runningUserId.c_str(), userid_length);
    __a2e_l(userid_buffer, userid_length);
  }

  build_attribute("requestid", adminType + "_request");

  if ((request.contains("traits")) && (!request["traits"].empty())) {
    build_end_nested_tag();

    *errors = build_request_data(adminType, request["traits"]);

    // Close the admin object
    build_full_close_tag(adminType);

    // Close the securityrequest tag (Consistent)
    build_full_close_tag("securityrequest");
  } else {
    // Close the admin object
    build_close_tag_no_value();
  }

  if (*debug) {
    // print information in debug mode
    std::cout << "XML Request string (Ascii): " << xml_buffer << "\n";
  }

  // convert our c++ string to a char * buffer
  const int length = xml_buffer.length();
  char* output_buffer = static_cast<char*>(malloc(sizeof(char) * (length + 1)));
  strncpy(output_buffer, xml_buffer.c_str(), length + 1);
  __a2e_l(output_buffer, length);

  *request_length = length;

  if (*debug) {
    // print information in debug mode
    std::cout << std::hex << "XML Request string (Ebcdic): " << std::hex
              << cast_hex_string(output_buffer) << "\n";
  }

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

void XmlGenerator::build_xml_head_attributes(std::string adminType,
                                             nlohmann::json request,
                                             int* irrsmo00_options) {
  // Obtain JSON Header information and Build into Admin Object where
  // appropriate

  std::string className, operation;

  operation = convert_operation(request["operation"].get<std::string>(),
                                irrsmo00_options);
  build_attribute("operation", operation);
  if (request.contains("run")) {
    build_attribute("run", request["run"].get<std::string>());
  }
  if (adminType == "systemsettings") {
    return;
  }
  if (request.contains("override")) {
    build_attribute("override", request["override"].get<std::string>());
  }
  build_attribute("name", request["profile_name"].get<std::string>());
  if ((adminType == "user") || (adminType == "group")) {
    return;
  }
  if (adminType == "groupconnection") {
    build_attribute("group", request["group"].get<std::string>());
    return;
  }
  if ((adminType == "resource") || (adminType == "permission")) {
    className = request["class"].get<std::string>();
    build_attribute("class", className);
    if (adminType == "resource" || (className != "dataset")) {
      return;
    }
  }
  if ((adminType == "dataset") || (adminType == "permission")) {
    if (request.contains("volume")) {
      build_attribute("volume", request["volume"].get<std::string>());
    }
    if (request.contains("generic")) {
      build_attribute("generic", request["generic"].get<std::string>());
    }
    return;
  }
  return;
}

nlohmann::json XmlGenerator::build_request_data(std::string adminType,
                                                nlohmann::json requestData) {
  // Builds the xml for request data (segment-trait information) passed in a
  // json object
  nlohmann::json errors;
  std::string currentSegment = "", itemSegment, itemTrait, itemOperation;
  const char* translatedKey;

  std::regex segment_trait_key_regex{R"~((([a-z]*):*)([a-z]*):(.*))~"};
  std::smatch segment_trait_key_data;

  auto item = requestData.begin();
  while (!requestData.empty()) {
    for (item = requestData.begin(); item != requestData.end();) {
      if (!regex_match(item.key(), segment_trait_key_data,
                       segment_trait_key_regex)) {
        // Track any entries that do not match proper syntax
        update_error_json(&errors, BAD_TRAIT_STRUCTURE,
                          nlohmann::json{
                              {"trait", item.key()}
        });
        item = requestData.erase(item);
        continue;
      }
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
        int8_t operation = map_operations(itemOperation);
        if (operation == OPERATOR_BAD) {
          update_error_json(&errors, BAD_OPERATION,
                            nlohmann::json{
                                {"operation", itemOperation}
          });
          item = requestData.erase(item);
          continue;
        }
        // I'm going to want to use item here rather than itemTrait. Not sure
        // how to type that? Maybe the JSON object?
        int8_t trait_type = map_trait_type(item.value());
        int8_t expected_type =
            get_racf_trait_type(adminType.c_str(), itemSegment.c_str(),
                                (itemSegment + ":" + itemTrait).c_str());
        if (expected_type == TRAIT_TYPE_BAD) {
          update_error_json(
              &errors, BAD_SEGMENT_TRAIT_COMBO,
              nlohmann::json{
                  {"segment", itemSegment},
                  {  "trait",   itemTrait}
          });
          item = requestData.erase(item);
          continue;
        }
        if (trait_type != expected_type) {
          update_error_json(
              &errors, BAD_TRAIT_DATA_TYPE,
              nlohmann::json{
                  {        "trait",    item.key()},
                  {"required_type", expected_type}
          });
          item = requestData.erase(item);
          continue;
        }
        translatedKey = get_racf_key(adminType.c_str(), itemSegment.c_str(),
                                     (itemSegment + ":" + itemTrait).c_str(),
                                     trait_type, operation);
        if (translatedKey == NULL) {
          update_error_json(&errors, BAD_TRAIT_OPERATION_COMBO,
                            nlohmann::json{
                                {"operation", itemOperation},
                                {  "segment",   itemSegment},
                                {    "trait",     itemTrait}
          });
          item = requestData.erase(item);
          continue;
        }
        std::string operation_str =
            (itemOperation.empty()) ? "set" : itemOperation;
        std::string value =
            (item.value().is_boolean())
                ? ""
                : json_value_to_string(item.value(), expected_type, &errors);
        build_single_trait(("racf:" + std::string(translatedKey)),
                           operation_str, value);
        item = requestData.erase(item);

      } else
        item++;
    }
    build_full_close_tag(currentSegment);
    currentSegment = "";
  }
  return errors;
}

int8_t XmlGenerator::map_operations(std::string operation) {
  if (operation.empty()) {
    return OPERATOR_ANY;
  }
  std::transform(operation.begin(), operation.end(), operation.begin(),
                 ::tolower);
  if (operation == "set") {
    return OPERATOR_SET;
  }
  if (operation == "add") {
    return OPERATOR_ADD;
  }
  if ((operation == "remove") || (operation == "rem")) {
    return OPERATOR_REMOVE;
  }
  if ((operation == "delete") || (operation == "del")) {
    return OPERATOR_DELETE;
  }
  return OPERATOR_BAD;
}

int8_t XmlGenerator::map_trait_type(const nlohmann::json& trait) {
  if (trait.is_boolean() || trait.is_null()) {
    return TRAIT_TYPE_BOOLEAN;
  }
  if (trait.is_string() || trait.is_array()) {
    return TRAIT_TYPE_STRING;
  }
  if (trait.is_number_unsigned()) {
    return TRAIT_TYPE_UINT;
  }
  if (trait.is_object() || trait.is_number()) {
    return TRAIT_TYPE_BAD;
  }
  return TRAIT_TYPE_ANY;
}

std::string XmlGenerator::json_value_to_string(const nlohmann::json& trait,
                                               char expected_type,
                                               nlohmann::json* errors) {
  if (trait.is_string()) {
    return trait.get<std::string>();
  }
  if (trait.is_array()) {
    std::string output_string = "";
    std::string delimeter =
        ", ";  // May just be " " or just be ","; May need to test
    for (const auto& item : trait.items()) {
      if (!item.value().is_string()) {
        update_error_json(
            errors, BAD_TRAIT_DATA_TYPE,
            nlohmann::json{
                {        "trait",    item.key()},
                {"required_type", expected_type}
        });
        return trait.dump();
      }
      output_string += item.value().get<std::string>() + delimeter;
    }
    for (int i = 0; i < delimeter.length(); i++) {
      output_string.pop_back();
    }
    return output_string;
  }
  return trait.dump();
}

std::string XmlGenerator::convert_operation(std::string requestOperation,
                                            int* irrsmo00_options) {
  // Converts the designated function to the correct IRRSMO00 operation and
  // adjusts IRRSMO00 options as necessary (alter operations require the
  // PRECHECK attribute)
  if (requestOperation == "add") {
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