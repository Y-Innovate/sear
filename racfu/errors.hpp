#ifndef __RACFU_ERRORS_H_
#define __RACFU_ERRORS_H_

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#define BAD_PARAMETER_VALUE 1  // "'junk' is not a valid value for 'admin_type'"
#define REQUIRED_PARAMETER 2   // "'operation' is a required parameter""
#define MISSING_PARAMETER \
  3  // "'class_name' is a required parameter for the 'resource' admin type"
#define BAD_PARAMETER_NAME 4       // "'junk' is not a valid parameter"
#define BAD_PARAMETER_DATA_TYPE 5  // "'admin_type' must be a string value"
#define BAD_TRAIT_STRUCTURE \
  6  //"'junk' is not in '<segment>:<trait>' or '<operation>:<segment>:<trait>'
     // format"
#define BAD_OPERATION 7            // "'junk' is not a valid trait operation"
#define BAD_TRAIT_DATA_TYPE 8      // "'omvs:uid' must be an 'integer' value"
#define BAD_SEGMENT_TRAIT_COMBO 9  // "'omvs:junk' is not a valid trait"
#define BAD_TRAIT_OPERATION_COMBO \
  10  //"'junk' must exist in the 'garbage' class before targeting with 'alter'"
#define BAD_ALTER_TARGET 11  //"'junk' must exist before targeting with 'alter'"
#define BAD_ALTER_TARGET_NO_CLASS \
  12  //"'remove' is not a valid operation for 'omvs:uid'"
#define XML_PARSE_ERROR 101  // "Unable to parse XML returned by IRRSMO00"
#define SMO_ERROR_NO_TEXT \
  102  // copied from "errormessage" supplied by irrsmo00
#define SMO_ERROR_WITH_TEXT \
  103  // copied from "errormessage" supplied by irrsmo00 plus "Text in error:
       // {textinerror}" supplied by irrsmo00

void update_error_json(nlohmann::json* errors_p, int8_t error_type,
                       nlohmann::json error_data);

nlohmann::json format_error_json(nlohmann::json* errors_p);

std::string decode_data_type(uint8_t data_type_code);

#endif
