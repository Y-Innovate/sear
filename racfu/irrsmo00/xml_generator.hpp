#ifndef __RACFU_XML_GENERATOR_H_
#define __RACFU_XML_GENERATOR_H_

#include <nlohmann/json.hpp>
#include <string>

#include "errors.hpp"
#include "logger.hpp"
#include "messages.h"
#include "security_request.hpp"

// XmlGenerator Generates an XML String from a JSON string
class XmlGenerator {
 private:
  std::string xml_buffer;
  static std::string replace_xml_chars(std::string data);
  void build_open_tag(std::string tag);
  void build_attribute(std::string name, std::string value);
  void build_value(std::string value);
  void build_end_nested_tag();
  void build_full_close_tag(std::string tag);
  void build_close_tag_no_value();
  void build_single_trait(const std::string& tag, const std::string& operation,
                          const std::string& value);
  void build_xml_header_attributes(const RACFu::SecurityRequest& request,
                                   const std::string& true_admin_type);
  void build_request_data(const std::string& true_admin_type,
                          const std::string& admin_type,
                          nlohmann::json request_data);
  static std::string convert_operation(const std::string& operation);
  static std::string convert_operator(const std::string& trait_operator);
  static std::string convert_admin_type(const std::string& admin_type);
  std::string json_value_to_string(const nlohmann::json& trait);

 public:
  void build_xml_string(const RACFu::SecurityRequest& request,
                        RACFu::Errors& errors, Logger& logger);
};

#endif
