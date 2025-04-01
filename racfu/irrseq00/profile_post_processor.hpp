#ifndef __RACFU_PROFILE_POST_PROCESSOR_H_
#define __RACFU_PROFILE_POST_PROCESSOR_H_

#include <nlohmann/json.hpp>
#include <string>

#include "irrseq00.hpp"

namespace RACFu {
class ProfilePostProcessor {
 public:
  explicit ProfilePostProcessor() {};
  static nlohmann::json post_process_generic(
      generic_extract_parms_results_t *generic_result_buffer,
      const std::string &admin_type);
  static nlohmann::json post_process_setropts(
      setropts_extract_results_t *setropts_result_buffer);

 private:
  static void process_setropts_field(char *field_data_destination,
                                     const char *field_data_source,
                                     int field_length);
  static void process_generic_field(nlohmann::json &json_field,
                                    generic_field_descriptor_t *field,
                                    char *field_key, char *profile_address,
                                    const char racfu_field_type);
  static char get_setropts_field_type(const char *field_key);
  static std::string post_process_field_key(char *field_key,
                                            const std::string &admin_type,
                                            const char *segment,
                                            const char *raw_field_key);
  static void post_process_key(char *destination_key, const char *source_key,
                               int length);
  static void copy_and_encode_string(char *destination_string,
                                     const char *source_string, int length);
  static void convert_to_lowercase(char *string, int length);
  static void trim_trailing_spaces(char *string, int length);
};
}  // namespace RACFu

#endif
