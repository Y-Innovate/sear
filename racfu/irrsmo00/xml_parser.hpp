#ifndef __RACFU_XML_PARSER_H_
#define __RACFU_XML_PARSER_H_

#include <nlohmann/json.hpp>
#include <string>

#include "logger.hpp"
#include "security_request.hpp"

namespace RACFu {
// XMLParser Parses an XML String and forms a JSON String
class XMLParser {
 private:
  void parse_xml_tags(nlohmann::json& input_json, std::string input_xml_string);
  void parse_xml_data(nlohmann::json& input_json,
                      const std::string& data_within_outer_tags,
                      const std::string& outer_tag);
  static void update_json(nlohmann::json& input_json,
                          nlohmann::json& inner_data, std::string outer_tag);
  static std::string replace_xml_chars(std::string xml_data);
  static std::string replace_substring(std::string data, std::string substring,
                                       std::string replacement,
                                       std::size_t start);

 public:
  nlohmann::json build_json_string(SecurityRequest& request);
};
}  // namespace RACFu

#endif
