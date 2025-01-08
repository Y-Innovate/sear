#ifndef __RACFU_ERRORS_H_
#define __RACFU_ERRORS_H_

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#define BAD_PARAMETER_VALUE 1
//"'junk' is not a valid value for 'admin_type'"
#define REQUIRED_PARAMETER 2
//"'operation' is a required parameter""
#define MISSING_PARAMETER 3
//"'class_name' is a required parameter for the 'resource' admin type"
#define BAD_PARAMETER_NAME 4
//"'junk' is not a valid parameter for the 'user' admin type"
#define BAD_PARAMETER_DATA_TYPE 5
//"'admin_type' must be a string value"
#define BAD_TRAIT_STRUCTURE 6
//"'junk' is not in '<segment>:<trait>' or '<operator>:<segment>:<trait>'
// format"
#define BAD_OPERATOR 7
//"'junk' is not a valid trait operator"
#define BAD_TRAIT_DATA_TYPE 8
//"'omvs:uid' must be an 'integer' value"
#define BAD_SEGMENT_TRAIT_COMBO 9
//"'omvs:junk' is not a valid trait"
#define BAD_TRAIT_OPERATOR_COMBO 10
//"'remove' is not a valid operator for 'omvs:uid'"
#define BAD_ALTER_TARGET_CLASS 11
//"Unable to alter 'junk' in the 'garbage' class because the profile does not
// exist"
#define BAD_ALTER_TARGET 12
//"unable to alter 'junk' because the profile does not exist"
#define BAD_VALUE_FOR_DELETE 13
//"'delete' operator for 'omvs:uid' can only be used with a 'null' value"
#define NULL_NOT_ALLOWED_TRAIT 14
//"'base:default_group_authority' can NOT be used with a 'null' value "
#define NULL_NOT_ALLOWED_OPERATOR 15
//"'set' operator for 'base:passphrase' can NOT be used with a 'null' value"
#define BAD_BOOLEAN_OPERATOR_COMBO 16
//"'false' is not a valid value for 'omvs:auto_uid'"
#define XML_PARSE_ERROR 101
//"unable to parse XML returned by IRRSMO00"
#define BAD_ADD_TARGET_CLASS 102
//"unable to add 'junk' to the 'garbage' class because a profile already exists
// with that name"
#define BAD_ADD_TARGET 103
//"unable to add 'junk' because a profile already exists with that name"
#define SMO_ERROR_NO_TEXT 111
// copied from "errormessage" supplied by irrsmo00
#define SMO_ERROR_WITH_TEXT 112
// copied from "errormessage" supplied by irrsmo00 plus "Text in error:
// {textinerror}" supplied by irrsmo00
#define EXTRACT_FAILED 113
// "unable to extract 'user' profile 'SQUIDWRD'""

void update_error_json(nlohmann::json* errors_p, int8_t error_type,
                       nlohmann::json error_data);

nlohmann::json format_error_json(nlohmann::json* errors_p);

std::string decode_data_type(uint8_t data_type_code);

#endif
