/* saf_xml.hpp */

#ifndef XML_PARSER_H_
#define XML_PARSER_H_

#ifndef XML_COMMON_LIB_H_
#include "../../externals/nlohmann/json.hpp"

std::string cast_hex_string(char* input);
#endif /* XML_COMMON_LIB_H_ */

// XmlParser Parses an XML String and forms a JSON String
class XmlParser {
 private:
  void parse_header_attributes(nlohmann::json* input_json,
                               std::string header_string);
  void parse_xml_tags(nlohmann::json* input_json, std::string body_string);
  void parse_xml_data(nlohmann::json* input_json, std::string inner_data,
                      std::string outer_tag);
  void update_json(nlohmann::json* input_json, nlohmann::json inner_data,
                   std::string outer_tag);
  std::string replace_xml_chars(std::string xml_data);
  std::string replace_substring(std::string data, std::string substring,
                                std::string replacement, std::size_t start);

 public:
  nlohmann::json build_json_string(char* xml_result_string, int* racfu_rc,
                                   bool debug);
};

#endif