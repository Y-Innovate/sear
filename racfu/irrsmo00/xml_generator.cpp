#include "xml_generator.hpp"

#include <unistd.h>

#include <iostream>
#include <regex>
#include <string>

#include "key_map.hpp"

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

  adminType = std::string(admin_type);
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

  (*errors) = build_xml_head_attributes(adminType, request, irrsmo00_options);

  if (!runningUserId.empty()) {
    // Run this command as another user id
    const int userid_length = runningUserId.length();
    strncpy(userid_buffer, runningUserId.c_str(), userid_length);
    __a2e_l(userid_buffer, userid_length);
  }

  build_attribute("requestid", adminType + "_request");

  if ((request.contains("traits")) && (!request["traits"].empty())) {
    build_end_nested_tag();

    (*errors).merge_patch(build_request_data(adminType, request["traits"]));

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

nlohmann::json XmlGenerator::build_xml_head_attributes(std::string adminType,
                                                       nlohmann::json request,
                                                       int* irrsmo00_options) {
  // Obtain JSON Header information and Build into Admin Object where
  // appropriate

  std::string requestOperation, operation, run, override_str, profileName,
      group, className, generic, volume;
  nlohmann::json errors;
  nlohmann::json yes_or_no = {"yes", "no"};
  nlohmann::json yes_no_or_force = {"yes", "no", "force"};
  nlohmann::json no_validation = {};

  pull_attribute_add_to_header(&request, &errors, "operation", "operation",
                               no_validation, true, irrsmo00_options);

  pull_attribute_add_to_header(&request, &errors, "run", "run", yes_or_no,
                               false, irrsmo00_options);
  if (adminType.compare("systemsettings") == 0) {
    return validate_remaining_request_attributes(request, errors);
  }
  pull_attribute_add_to_header(&request, &errors, "override", "override",
                               yes_no_or_force, false, irrsmo00_options);
  pull_attribute_add_to_header(&request, &errors, "profile_name", "name",
                               no_validation, true, irrsmo00_options);
  if ((adminType.compare("user") == 0) || (adminType.compare("group") == 0)) {
    return validate_remaining_request_attributes(request, errors);
  }
  if (adminType.compare("groupconnection") == 0) {
    pull_attribute_add_to_header(&request, &errors, "group", "group",
                                 no_validation, true, irrsmo00_options);
    return validate_remaining_request_attributes(request, errors);
  }
  if ((adminType.compare("resource") == 0) ||
      (adminType.compare("permission") == 0)) {
    pull_attribute_add_to_header(&request, &errors, "class_name", "class",
                                 no_validation, true, irrsmo00_options);
    if (adminType.compare("resource") == 0 ||
        (className.compare("dataset") != 0)) {
      return validate_remaining_request_attributes(request, errors);
    }
  }
  if ((adminType.compare("dataset") == 0) ||
      (adminType.compare("permission") == 0)) {
    pull_attribute_add_to_header(&request, &errors, "volume", "volume",
                                 no_validation, false, irrsmo00_options);
    pull_attribute_add_to_header(&request, &errors, "generic", "generic",
                                 yes_or_no, false, irrsmo00_options);
    return validate_remaining_request_attributes(request, errors);
  }
  update_error_json(&errors, "bad_header_value", "admin_type:" + adminType);
  return errors;
}

void XmlGenerator::pull_attribute_add_to_header(
    nlohmann::json* request, nlohmann::json* errors, std::string json_key,
    std::string xml_key, nlohmann::json validation, bool required,
    int* irrsmo00_options) {
  std::string val;
  if ((*request).contains(json_key)) {
    val = (*request)[json_key].get<std::string>();
    std::transform(val.begin(), val.end(), val.begin(), ::tolower);
    (*request).erase(json_key);
    if (validation.empty()) {
      if (json_key.compare("operation") == 0) {
        std::string operation_string;
        operation_string = convert_operation(val, irrsmo00_options);
        if (val.empty()) {
          update_error_json(errors, "bad_header_value", json_key + ":" + val);
          return;
        }
        val = operation_string;
      }
      build_attribute(xml_key, val);

    } else {
      for (auto& item : validation.items()) {
        if (val.compare(item.value()) == 0) {
          build_attribute(xml_key, val);
          return;
        }
      }
      update_error_json(errors, "bad_header_value", json_key + ":" + val);
    }
  } else {
    if (required) {
      update_error_json(errors, "missing_header_attribute", json_key);
    }
  }
}

nlohmann::json XmlGenerator::validate_remaining_request_attributes(
    nlohmann::json request, nlohmann::json errors) {
  for (auto& item : request.items()) {
    // Traits contain no Header information and is ignored
    if (item.key().compare("traits") == 0) {
      continue;
    } else if (item.key().compare("admin_type") == 0) {
      // The type of administrative object we are working with
      continue;
    }
    // Anything else shouldn't be here
    update_error_json(&errors, "bad_header_name", item.key());
  }
  return errors;
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
        update_error_json(&errors, "bad_structure", item.key());
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
          update_error_json(&errors, "bad_operation", itemOperation);
          item = requestData.erase(item);
          continue;
        }
        // I'm going to want to use item here rather than itemTrait. Not sure
        // how to type that? Maybe the JSON object?
        int8_t trait_type = map_trait_type(item.value());
        if (trait_type == TRAIT_TYPE_BAD) {
          update_error_json(&errors, "bad_trait_or_trait_type",
                            json_value_to_string(item.value(), &errors));
        }
        translatedKey = get_racf_key(adminType.c_str(), itemSegment.c_str(),
                                     (itemSegment + ":" + itemTrait).c_str(),
                                     trait_type, operation);
        if (translatedKey == NULL) {
          update_error_json(&errors, "bad_segment_trait_operation_combination",
                            (itemSegment + ":" + itemTrait));
        }
        std::string operation_str =
            (itemOperation.empty()) ? "set" : itemOperation;
        std::string value = (item.value().is_boolean())
                                ? ""
                                : json_value_to_string(item.value(), &errors);
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
  if (operation.compare("set")) {
    return OPERATOR_SET;
  }
  if (operation.compare("add")) {
    return OPERATOR_ADD;
  }
  if (operation.compare("remove") || operation.compare("rem")) {
    return OPERATOR_REMOVE;
  }
  if (operation.compare("delete") || operation.compare("del")) {
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
  if (trait.is_number()) {
    return TRAIT_TYPE_INTEGER;
  }
  if (trait.is_object()) {
    return TRAIT_TYPE_BAD;
  }
  return TRAIT_TYPE_ANY;
}

std::string XmlGenerator::json_value_to_string(const nlohmann::json& trait,
                                               nlohmann::json* errors) {
  if (trait.is_string()) {
    return trait.get<std::string>();
  }
  if (trait.is_array()) {
    std::string output_string = "";
    std::string delimeter =
        ", ";  // May just be " " or just be ","; May need to test
    for (auto& item : trait.items()) {
      if (!item.value().is_string()) {
        update_error_json(errors, "bad_trait_or_trait_type", trait.dump());
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
  if (requestOperation.compare("add") == 0) {
    return "set";
  }
  if (requestOperation.compare("alter") == 0) {
    *irrsmo00_options = 15;
    return "set";
  }
  if (requestOperation.compare("delete") == 0) {
    return "del";
  }
  if (requestOperation.compare("extract") == 0) {
    return "listdata";
  }
  return "";
}

/*
void XmlGenerator::convert_to_ebcdic(char* ascii_str, int length) {
// Universal function to convert ascii string to EBCDIC-1047 in-place
#ifndef __MVS__
  for (int i = 0; i < length; i++) {
    *(ascii_str + i) = AsciiToEbcdic[(unsigned char)*(ascii_str + i)];
  }
#else
  __a2e_s(ascii_str);
#endif  //__MVS__
}
*/

void update_error_json(nlohmann::json* errors, std::string error_type,
                       std::string error_data) {
  if ((*errors).empty()) {
    (*errors)["errors"] = {};
  }
  if ((*errors)["errors"].contains(error_type)) {
    (*errors)["errors"][error_type].push_back(error_data);
  } else {
    (*errors)["errors"][error_type] = {error_data};
  }
}