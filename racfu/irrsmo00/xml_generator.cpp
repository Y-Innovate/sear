#include "xml_generator.hpp"

#include <regex>

#include "key_map.hpp"
#include "logger.hpp"
#include "racfu_error.hpp"
#include "trait_validation.hpp"

#ifdef __TOS_390__
#include <unistd.h>
#else
#include "zoslib.h"
#endif

namespace RACFu {
// Public Functions of XMLGenerator
void XMLGenerator::build_xml_string(SecurityRequest& request) {
  // Main body function that builds an xml string

  // Build the securityrequest tag (Consistent)
  build_open_tag("securityrequest");
  build_attribute("xmlns", "http://www.ibm.com/systems/zos/saf");
  build_attribute("xmlns:racf", "http://www.ibm.com/systems/zos/racf");
  build_end_nested_tag();

  std::string true_admin_type = convert_admin_type(request.admin_type_);
  build_open_tag(true_admin_type);

  build_xml_header_attributes(request, true_admin_type);

  build_attribute("requestid", true_admin_type + "_request");

  if (!request.traits_.empty()) {
    build_end_nested_tag();

    Logger::getInstance().debug("Validating traits ...");

    validate_traits(request.admin_type_, request);

    build_request_data(true_admin_type, request.admin_type_, request.traits_);

    Logger::getInstance().debug("Done");

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

  Logger::getInstance().debug("Request XML:", xml_buffer_);

  // convert our c++ string to a char * buffer
  const int length = xml_buffer_.length();
  request.p_result_->raw_request =
      static_cast<char*>(malloc(sizeof(char) * (length + 1)));
  if (request.p_result_->raw_request == NULL) {
    request.return_codes_.racfu_return_code = 8;
    throw RACFuError("Unable to allocate request buffer for IRRSMO00");
  }
  strncpy(request.p_result_->raw_request, xml_buffer_.c_str(), length + 1);
  __a2e_l(request.p_result_->raw_request, length);

  request.p_result_->raw_request_length = length;

  Logger::getInstance().debug(
      "EBCDIC encoded request XML:",
      Logger::getInstance().castHexString(request.p_result_->raw_request));
}

// Private Functions of XMLGenerator
std::string XMLGenerator::replace_xml_chars(std::string data) {
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
void XMLGenerator::build_open_tag(std::string tag) {
  // Ex: "<base:universal_access"
  tag = replace_xml_chars(tag);
  xml_buffer_.append("<" + tag);
}
void XMLGenerator::build_attribute(std::string name, std::string value) {
  // Ex: " operation=set"
  name  = replace_xml_chars(name);
  value = replace_xml_chars(value);
  xml_buffer_.append(" " + name + "=\"" + value + "\"");
}
void XMLGenerator::build_value(std::string value) {
  // Ex: ">Read"
  value = replace_xml_chars(value);
  xml_buffer_.append(">" + value);
}
void XMLGenerator::build_end_nested_tag() { xml_buffer_.append(">"); }
void XMLGenerator::build_full_close_tag(std::string tag) {
  // Ex: "</base:universal_access>"
  tag = replace_xml_chars(tag);
  xml_buffer_.append("</" + tag + ">");
}
void XMLGenerator::build_close_tag_no_value() { xml_buffer_.append("/>"); }
void XMLGenerator::build_single_trait(const std::string& tag,
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

void XMLGenerator::build_xml_header_attributes(
    const SecurityRequest& request, const std::string& true_admin_type) {
  // Obtain JSON Header information and Build into Admin Object where
  // appropriate
  if (request.operation_ == "add") {
    build_attribute("override", "no");
  }
  std::string irrsmo00_operation = convert_operation(request.operation_);
  build_attribute("operation", irrsmo00_operation);
  /*
  if (request.contains("run")) {
    build_attribute("run", request["run"].get<std::string>());
  }
  */
  if (true_admin_type == "systemsettings") {
    return;
  }
  build_attribute("name", request.profile_name_);
  if ((true_admin_type == "user") || (true_admin_type == "group")) {
    return;
  }
  if (true_admin_type == "groupconnection") {
    build_attribute("group", request.group_);
    return;
  }
  if ((true_admin_type == "resource") || (true_admin_type == "permission")) {
    build_attribute("class", request.class_name_);
  }
  if ((true_admin_type == "dataset") || (true_admin_type == "permission")) {
    if (!request.volume_.empty()) {
      build_attribute("volume", request.volume_);
    }
    if (!request.generic_.empty()) {
      build_attribute("generic", request.generic_);
    }
    return;
  }
  return;
}

void XMLGenerator::build_request_data(const std::string& true_admin_type,
                                      const std::string& admin_type,
                                      nlohmann::json request_data) {
  // Builds the xml for request data (segment-trait information) passed in a
  // json object
  nlohmann::json built_request{};
  std::string current_segment = "", item_segment, item_trait, item_operator;
  const char *translated_key, *racf_field_key;

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
        int8_t trait_type    = map_trait_type(item.value());
        int8_t expected_type = get_trait_type(admin_type, item_segment,
                                              item_segment + ":" + item_trait);
        if (expected_type == TRAIT_TYPE_PSEUDO_BOOLEAN and
            trait_type != TRAIT_TYPE_NULL) {
          trait_type = TRAIT_TYPE_PSEUDO_BOOLEAN;
        }
        translated_key = get_racf_key(admin_type.c_str(), item_segment.c_str(),
                                      (item_segment + ":" + item_trait).c_str(),
                                      trait_type, trait_operator);
        if (translated_key == nullptr) {
          // Temporary to get list/repeat traits working for RACF Options
          translated_key =
              get_racf_key(admin_type.c_str(), item_segment.c_str(),
                           (item_segment + ":" + item_trait).c_str(),
                           TRAIT_TYPE_REPEAT, trait_operator);
        }
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
          case TRAIT_TYPE_PSEUDO_BOOLEAN:
            trait_operator_str = "set";
            value              = (item.value()) ? "YES" : "NO";
            break;
          default:
            trait_operator_str = (item_operator.empty())
                                     ? "set"
                                     : convert_operator(item_operator);
            value              = (trait_type == TRAIT_TYPE_BOOLEAN)
                                     ? ""
                                     : json_value_to_string(item.value());
        }
        racf_field_key =
            (!(*(translated_key + strlen(translated_key) - 1) == '*'))
                ? translated_key
                : item_trait.c_str();
        build_single_trait(("racf:" + std::string(racf_field_key)),
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

std::string XMLGenerator::convert_operation(const std::string& operation) {
  // Converts the designated function to the correct IRRSMO00 operation.
  if (operation == "add") {
    return "set";
  }
  if (operation == "alter") {
    return "set";
  }
  if (operation == "delete") {
    return "del";
  }
  if (operation == "extract") {
    return "listdata";
  }
  return "";
}

std::string XMLGenerator::convert_operator(const std::string& trait_operator) {
  // Converts the designated function to the correct IRRSMO00 operator
  if (trait_operator == "delete") {
    return "del";
  }
  return trait_operator;
}

std::string XMLGenerator::convert_admin_type(const std::string& admin_type) {
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

std::string XMLGenerator::json_value_to_string(const nlohmann::json& trait) {
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
}  // namespace RACFu
