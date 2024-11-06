#include "xml_parser.hpp"

#include <unistd.h>

#include <iostream>
#include <regex>
#include <string>

#include "error_validation.hpp"

// Public Methods of XmlParser
nlohmann::json XmlParser::build_json_string(char* xml_result_string,
                                            int* racfu_rc, bool debug) {
  std::string xml_buffer;
  char* xml_ascii_result =
      static_cast<char*>(calloc(strlen(xml_result_string) + 1, sizeof(char)));

  // Build a JSON string from the XML result string, SMO return and Reason
  // Codes
  if (debug) {
    // print information in debug mode
    std::cout << "XML Result string (Ebcdic): " << std::hex
              << cast_hex_string(xml_result_string) << "\n";
  }

  int xml_result_length = strlen(xml_result_string);
  memcpy(xml_ascii_result, xml_result_string, xml_result_length);
  __e2a_l(xml_ascii_result, xml_result_length);
  xml_buffer = xml_ascii_result;

  if (debug) {
    // print information in debug mode
    std::cout << "XML Result string (Ascii): " << xml_buffer << "\n";
  }

  // Regular expression designed to match the parameter, generic body, and
  // closing tags of the xml
  std::regex full_xml_regex{
      R"~(<\?xml version="1\.0" encoding="IBM-1047"\?><securityresult xmlns="http:\/\/www\.ibm\.com\/systems\/zos\/saf\/IRRSMO00Result1"><([a-z]*) ([^>]*)>(<.+>)<\/securityresult>)~"};
  std::smatch useful_xml_substrings;

  nlohmann::json result_json;
  nlohmann::json result;

  std::string admin_type, admin_close_tag, admin_xml_attrs, admin_xml_body;

  if (regex_match(xml_buffer, useful_xml_substrings, full_xml_regex)) {
    // Use sub-matches in the regular expression to pull out useful
    // information
    admin_type = useful_xml_substrings[1];
    admin_xml_attrs = useful_xml_substrings[2];
    admin_xml_body = useful_xml_substrings[3];

    parse_parameters(&result, admin_xml_attrs);

    // Erase the profile close tag as it messes up later regex parsing
    admin_close_tag = R"(</)" + admin_type + ">";
    admin_xml_body.erase(admin_xml_body.find(admin_close_tag),
                         admin_close_tag.length());

    parse_xml_tags(&result, admin_xml_body);

    result_json["admin_type"] = admin_type;
    result_json["result"] = result;
    *racfu_rc = 0;
  } else {
    // If the XML does not match the main regular expression, then return
    // this string to indicate an error
    update_error_json(&result_json["errors"], XML_PARSE_ERROR,
                      {
                          {"xml_data", xml_buffer}
    });
    *racfu_rc = 4;
  }

  free(xml_ascii_result);
  return result_json;
}

// Private Methods of XmlParser
void XmlParser::parse_parameters(nlohmann::json* input_json,
                                 std::string parameter_string) {
  // Parse the parameter attributes of the XML for JSON information
  // Ex: name="SQUIDWRD" operation="set" requestid="UserRequest"
  std::smatch attribute_key_value;
  std::regex attribute_regex{R"~(([a-z]*)="([^ ]*)")~"};

  std::string::size_type attribute_length = -1, attribute_start_index = 0,
                         parameter_string_length;
  std::string remaining_parameter_string, attribute_string, attribute_key,
      attribute_value;

  parameter_string_length = parameter_string.length();
  do {
    // Loop through XML attributes and add them to the input_json JSON
    // object
    attribute_start_index += attribute_length + 1;
    remaining_parameter_string = parameter_string.substr(
        attribute_start_index, parameter_string_length - attribute_start_index);
    attribute_length = remaining_parameter_string.find(' ');
    attribute_string =
        parameter_string.substr(attribute_start_index, attribute_length);

    if (regex_match(attribute_string, attribute_key_value, attribute_regex)) {
      // Ex: 1) name 2) SQUIDWRD
      attribute_key = attribute_key_value[1];
      attribute_value = attribute_key_value[2];
      (*input_json)[attribute_key] = attribute_value;
    }
  } while (attribute_start_index !=
           attribute_length + attribute_start_index + 1);
};

void XmlParser::parse_xml_tags(nlohmann::json* input_json,
                               std::string input_xml_string) {
  // Parse the outer layer of the XML (the tags) for attributes and tag names
  // with regex Ex:
  // <safreturncode>0</safreturncode><returncode>0</returncode><reasoncode>0</reasoncode><image>ADDUSER
  // SQUIDWRD </image><message>ICH01024I User SQUIDWRD is defined as
  // PROTECTED.</message>
  std::regex outermost_xml_tags_regex{R"(<([a-z]*)>.*</([a-z]*)>)"};
  std::smatch outermost_xml_tags, next_xml_tag, data_around_current_tag;

  std::string::size_type start_index = 0, end_index;
  std::string current_tag, remaining_string, data_within_current_tags;

  // If we do not match our generic regular expression for an XML attribute
  if (!regex_match(input_xml_string, outermost_xml_tags,
                   outermost_xml_tags_regex)) {
    return;
  }
  // Use regex substrings to identify the name of the current xml tag
  // Ex: safreturncode
  current_tag = outermost_xml_tags[1];
  while (current_tag != "") {
    // Enter a loop iterating through xml looking for XML tags within the
    // "current" tag In a practical sense, from SMO this ends up parsing
    // "Command" entries, then looking At individual xml entries within
    // these "Command" entries like "image" or "message"
    std::regex current_tags_regex{"<" + current_tag + R"~(>(.*?)(<\/)~" +
                                  current_tag + ">)(<(.*?)>)?.*"};
    remaining_string = input_xml_string.substr(start_index);
    if (regex_match(remaining_string, data_around_current_tag,
                    current_tags_regex)) {
      // Ex: 1) 0 2) </safreturncode> 3) <returncode>  4) returncode
      data_within_current_tags = data_around_current_tag[1];
      parse_xml_data(input_json, data_within_current_tags, current_tag);
      start_index += current_tag.length() * 2 +
                     ((std::string) "<></>").length() +
                     data_within_current_tags.length();
      // Update current tag with the "next" tag found after the current
      // set
      current_tag = data_around_current_tag[4];
    }
  }
};

void XmlParser::parse_xml_data(nlohmann::json* input_json,
                               std::string data_within_outer_tags,
                               std::string outer_tag) {
  // Parse data from within XML tags and add the values to the JSON
  if (data_within_outer_tags.find("<") == std::string::npos) {
    update_json(input_json, data_within_outer_tags, outer_tag);
    return;
  }
  // If we did not return, there is another xml tag within this data (nested)
  nlohmann::json nested_json;
  std::string nested_xml = data_within_outer_tags;
  parse_xml_tags(&nested_json, nested_xml);
  update_json(input_json, nested_json, outer_tag);
}

void XmlParser::update_json(nlohmann::json* input_json,
                            nlohmann::json inner_data, std::string outer_tag) {
  // Add specified information (inner_data) to the input_json JSON object
  // using the specified key (outer_tag)
  outer_tag = replace_xml_chars(outer_tag);
  if (inner_data.is_string()) {
    inner_data = replace_xml_chars(inner_data);
  }
  if (!((*input_json).contains(outer_tag) ||
        (*input_json).contains(outer_tag + "s"))) {
    // If we do not already have this tag used in our object (at this
    // layer), just add data
    (*input_json)[outer_tag] = inner_data;
    return;
  }
  if ((*input_json).contains(outer_tag)) {
    // If we do already use this tag, pluralize the tag and merge the data
    (*input_json)[outer_tag + "s"] = {(*input_json)[outer_tag], inner_data};
    (*input_json).erase(outer_tag);
  } else {
    // If we already have the plural version of this tag, simply merge the
    // data
    (*input_json)[outer_tag + "s"].push_back(inner_data);
  }
}

std::string XmlParser::replace_xml_chars(std::string xml_data) {
  std::string amp = "&amp;", gt = "&gt;", lt = "&lt;", quot = "&quot;",
              apos = "&apos;";
  std::size_t index;

  index = xml_data.find("&");
  if (index == std::string::npos) {
    return xml_data;
  }
  do {
    xml_data = replace_substring(xml_data, gt, ">", index);
    xml_data = replace_substring(xml_data, lt, "<", index);
    xml_data = replace_substring(xml_data, apos, "'", index);
    xml_data = replace_substring(xml_data, quot, "\"", index);
    xml_data = replace_substring(xml_data, amp, "&", index);
    index = xml_data.find("&", index + 1);
  } while (index != std::string::npos);
  return xml_data;
}

std::string XmlParser::replace_substring(std::string data,
                                         std::string substring,
                                         std::string replacement,
                                         std::size_t start) {
  std::size_t match;
  if ((data.length() - start) < substring.length()) {
    return data;
  }
  match = data.find(substring, start);
  if (match == std::string::npos) {
    return data;
  }
  data.replace(match, substring.length(), replacement);
  return data;
}

#ifndef XML_COMMON_LIB_H_
// Functions common to both XML libraries

std::string cast_hex_string(char* input) {
  // Cast data to hex so that small strings of hex values can be printed to
  // represent EBCDIC data
  std::string output = "{ ";
  char buff[5];
  for (int i = 0; i < strlen(input); i++) {
    if (i > 0) {
      output += ", ";
    }
    std::snprintf(buff, 5, "0x%02x", (unsigned char)*(input + i));
    output += buff;
  }
  output += " }";
  return output;
}
#endif /* XML_COMMON_LIB_H_ */