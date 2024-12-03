#ifndef __RACFU_XML_PARSER_H_
#define __RACFU_XML_PARSER_H_

#include <nlohmann/json.hpp>

#include "logger.hpp"
#include "messages.h"

// XmlParser Parses an XML String and forms a JSON String
class XmlParser {
 private:
  void parse_xml_tags(nlohmann::json* input_json_p, std::string body_string);
  void parse_xml_data(nlohmann::json* input_json_p, std::string inner_data,
                      std::string outer_tag);
  void update_json(nlohmann::json* input_json_p, nlohmann::json& inner_data,
                   std::string outer_tag);
  std::string replace_xml_chars(std::string xml_data);
  std::string replace_substring(std::string data, std::string substring,
                                std::string replacement, std::size_t start);

 public:
  nlohmann::json build_json_string(char* xml_result_string, int* racfu_rc_p,
                                   Logger* logger_p);
};

#endif
