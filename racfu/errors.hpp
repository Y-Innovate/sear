#ifndef __RACFU_ERRORS_H_
#define __RACFU_ERRORS_H_

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#define BAD_PARAMETER_VALUE 1  // "'junk' is not a valid value for 'admin_type'"
#define REQUIRED_PARAMETER 2   // "'operation' is a required parameter""
#define MISSING_PARAMETER \
  3  // "'class_name' is a required parameter for the 'resource' admin_type"
#define BAD_PARAMETER_NAME 4       // "'junk' is not a valid parameter"
#define BAD_PARAMETER_DATA_TYPE 5  // "'admin_type' must be a string value"
#define BAD_TRAIT_STRUCTURE \
  6  //"'junk' is not in '<segment>:<trait>' or '<operation>:<segment>:<trait>'
     // format"
#define BAD_OPERATION 7            // "'junk' is not a valid trait operation"
#define BAD_TRAIT_DATA_TYPE 8      // "'omvs:uid' must be an 'integer' value"
#define BAD_SEGMENT_TRAIT_COMBO 9  // "'omvs:junk' is not a valid trait"
#define BAD_TRAIT_OPERATION_COMBO \
  10  //"'remove' is not a valid operation for 'omvs:uid'"
#define XML_PARSE_ERROR 101  // "Unable to parse XML returned by IRRSMO00"

void update_error_json(nlohmann::json* errors, int8_t error_type,
                       nlohmann::json error_data);

nlohmann::json format_error_json(nlohmann::json* errors);

std::string decode_data_type(uint data_type_code);

#endif
