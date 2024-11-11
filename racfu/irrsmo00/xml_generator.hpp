/* saf_xml.hpp */

#ifndef XML_GENERATOR_H_
#define XML_GENERATOR_H_

#ifndef XML_COMMON_LIB_H_
#include <nlohmann/json.hpp>

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
  void build_xml_header_attributes(std::string adminType,
                                   nlohmann::json request,
                                   int *irrsmo00_options);
  nlohmann::json build_request_data(std::string adminType,
                                    nlohmann::json requestData);
  std::string convert_operation(std::string requestOperation,
                                int *irrsmo00_options);
  std::string convert_admin_type(std::string admin_type);
  std::string json_value_to_string(const nlohmann::json &trait);

 public:
  char *build_xml_string(const char *admin_type, nlohmann::json request,
                         nlohmann::json *errors, char *userid_buffer,
                         int *irrsmo00_options,
                         unsigned int *result_buffer_size,
                         unsigned int *request_length, bool *debug);
};

#endif
