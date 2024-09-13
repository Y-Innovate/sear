/* saf_xml.hpp */

#ifndef XML_GENERATOR_H_
#define XML_GENERATOR_H_

#ifndef XML_COMMON_LIB_H_
#include "../../externals/nlohmann/json.hpp"

std::string cast_hex_string(char *input);
#endif /* XML_COMMON_LIB_H_ */

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
  nlohmann::json build_xml_head_attributes(std::string adminType,
                                           nlohmann::json request,
                                           int *irrsmo00_options);
  void pull_attribute_add_to_header(nlohmann::json *request,
                                    nlohmann::json *errors,
                                    std::string json_key, std::string xml_key,
                                    nlohmann::json validation, bool required,
                                    int *irrsmo00_options);
  nlohmann::json validate_remaining_request_attributes(nlohmann::json request,
                                                       nlohmann::json errors);
  nlohmann::json build_request_data(std::string adminType,
                                    nlohmann::json requestData);
  int8_t map_operations(std::string operation);
  int8_t map_trait_type(const nlohmann::json &trait);
  std::string json_value_to_string(const nlohmann::json &trait,
                                   nlohmann::json *errors);
  std::string convert_operation(std::string requestOperation,
                                int *irrsmo00_options);
  void convert_to_ebcdic(char *ascii_str, int length);

 public:
  char *build_xml_string(const char *admin_type, nlohmann::json request,
                         nlohmann::json *errors, char *userid_buffer,
                         int *irrsmo00_options,
                         unsigned int *result_buffer_size,
                         unsigned int *request_length, bool *debug);
};

void update_error_json(nlohmann::json *errors, std::string error_type,
                       std::string error_data);

#endif