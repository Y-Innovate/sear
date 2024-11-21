#ifndef XML_GENERATOR_H_
#define XML_GENERATOR_H_

#include <nlohmann/json.hpp>

#include "logger.hpp"
#include "messages.h"

// XmlGenerator Generates an XML String from a JSON string
class XmlGenerator {
 private:
  std::string xml_buffer;
  std::string replace_xml_chars(std::string data);
  void build_open_tag(std::string tag);
  void build_attribute(std::string name, std::string value);
  void build_value(std::string value);
  void build_end_nested_tag();
  void build_full_close_tag(std::string tag);
  void build_close_tag_no_value();
  void build_single_trait(std::string tag, std::string operation,
                          std::string value);
  void build_xml_header_attributes(std::string true_admin_type,
                                   nlohmann::json *request_p,
                                   int *irrsmo00_options_p);
  nlohmann::json build_request_data(std::string true_admin_type,
                                    nlohmann::json request_data);
  std::string convert_operation(std::string request_operation,
                                int *irrsmo00_options_p);
  std::string convert_operator(std::string trait_operator);
  std::string convert_admin_type(std::string admin_type);
  std::string json_value_to_string(const nlohmann::json &trait);

 public:
  char *build_xml_string(const char *admin_type, nlohmann::json *request_p,
                         nlohmann::json *errors_p, char *userid_buffer,
                         int *irrsmo00_options_p,
                         unsigned int *request_length_p, Logger *logger_p);
};

#endif
