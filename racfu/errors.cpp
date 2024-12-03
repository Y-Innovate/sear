#include "errors.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#include "key_map.hpp"

void update_error_json(nlohmann::json* errors_p, int8_t error_type,
                       nlohmann::json error_data) {
  nlohmann::json error_json = {
      {"error_code", error_type},
      {"error_data", error_data}
  };

  if (error_type >= XML_PARSE_ERROR) {
    errors_p->push_back(error_json);
    return;
  }
  if ((*errors_p).empty()) {
    (*errors_p)["errors"] = nlohmann::json::array();
  }
  (*errors_p)["errors"].push_back(error_json);
}

nlohmann::json format_error_json(nlohmann::json* errors_p) {
  std::string error_message_str;
  std::string prefix = "racfu: ", smo_prefix = "irrsmo00: ";
  nlohmann::json error_data, output = {
                                 {"errors", {}}
  };
  for (int i = 0; i < errors_p->size(); i++) {
    if (!(*errors_p)[i]["error_data"].empty()) {
      error_data = (*errors_p)[i]["error_data"];
    }
    switch ((*errors_p)[i]["error_code"].get<uint>()) {
      case BAD_PARAMETER_VALUE:
        error_message_str = prefix + "'" +
                            error_data["parameter_value"].get<std::string>() +
                            "' is not a valid value for '" +
                            error_data["parameter"].get<std::string>() + "'";
        break;
      case REQUIRED_PARAMETER:
        error_message_str = prefix + "'" +
                            error_data["parameter"].get<std::string>() +
                            "' is a required parameter";
        break;
      case MISSING_PARAMETER:
        error_message_str =
            prefix + "'" + error_data["parameter"].get<std::string>() +
            "' is a required parameter for the '" +
            error_data["admin_type"].get<std::string>() + "' admin type";
        break;
      case BAD_PARAMETER_NAME:
        error_message_str = prefix + "'" +
                            error_data["parameter"].get<std::string>() +
                            "' is not a valid parameter";
        break;
      case BAD_PARAMETER_DATA_TYPE:
        error_message_str =
            prefix + "'" + error_data["parameter"].get<std::string>() +
            "' must be a " + error_data["data_type"].get<std::string>() +
            " value";
        break;
      case BAD_TRAIT_STRUCTURE:
        error_message_str = prefix + "'" +
                            error_data["trait"].get<std::string>() +
                            "' is not in '<segment>:<trait>' or "
                            "'<operator>:<segment>:<trait>' format";
        break;
      case BAD_OPERATOR:
        error_message_str = prefix + "'" +
                            error_data["operator"].get<std::string>() +
                            "' is not a valid trait operator";
        break;
      case BAD_TRAIT_DATA_TYPE:
        error_message_str =
            prefix + "'" + error_data["trait"].get<std::string>() +
            "' must be " +
            decode_data_type(error_data["required_type"].get<uint>()) +
            "' value";
        break;
      case BAD_SEGMENT_TRAIT_COMBO:
        error_message_str =
            prefix + "'" + error_data["segment"].get<std::string>() + ":" +
            error_data["trait"].get<std::string>() + "' is not a valid trait";
        break;
      case BAD_TRAIT_OPERATOR_COMBO:
        error_message_str = prefix + "'" +
                            error_data["operator"].get<std::string>() +
                            "' is not a valid operator for '" +
                            error_data["segment"].get<std::string>() + ":" +
                            error_data["trait"].get<std::string>() + "'";
        break;
      case BAD_ALTER_TARGET_CLASS:
        error_message_str = prefix + "Unable to alter '" +
                            error_data["name"].get<std::string>() +
                            "' in the '" +
                            error_data["class"].get<std::string>() +
                            "' class because the profile does not exist";
        break;
      case BAD_ALTER_TARGET:
        error_message_str = prefix + "unable to alter '" +
                            error_data["name"].get<std::string>() +
                            "' because the profile does not exist";
        break;
      case BAD_VALUE_FOR_DELETE:
        error_message_str = prefix + "'delete' operator for '" +
                            error_data["segment"].get<std::string>() + ":" +
                            error_data["trait"].get<std::string>() +
                            "' can only be used with a 'null' value";
        break;
      case NULL_NOT_ALLOWED_TRAIT:
        error_message_str = prefix + "'" +
                            error_data["segment"].get<std::string>() + ":" +
                            error_data["trait"].get<std::string>() +
                            "' can NOT be used with a 'null' value";
        break;
      case NULL_NOT_ALLOWED_OPERATOR:
        error_message_str = prefix + "'" +
                            error_data["operator"].get<std::string>() +
                            "' operator for '" +
                            error_data["segment"].get<std::string>() + ":" +
                            error_data["trait"].get<std::string>() +
                            +"' can NOT be used with a 'null' value";
        break;
      case XML_PARSE_ERROR:
        error_message_str = prefix + "unable to parse XML returned by IRRSMO00";
        break;
      case BAD_ADD_TARGET_CLASS:
        error_message_str =
            prefix + "unable to add '" + error_data["name"].get<std::string>() +
            "' in the '" + error_data["class"].get<std::string>() +
            "' class because a profile already exists with that name";
        break;
      case BAD_ADD_TARGET:
        error_message_str = prefix + "unable to add '" +
                            error_data["name"].get<std::string>() +
                            "' because a profile already exists with that name";
        break;
      case SMO_ERROR_NO_TEXT:
        error_message_str =
            smo_prefix + error_data["error_message"].get<std::string>();
        break;
      case SMO_ERROR_WITH_TEXT:
        error_message_str =
            smo_prefix + error_data["error_message"].get<std::string>() +
            " Text in error: " + error_data["text_in_error"].get<std::string>();
        break;
      default:
        error_message_str = prefix + "an unknown error has occurred";
    }
    output["errors"] += error_message_str;
  }
  return output;
}

std::string decode_data_type(uint8_t data_type_code) {
  switch (data_type_code) {
    case TRAIT_TYPE_BOOLEAN:
      return "a 'boolean";
    case TRAIT_TYPE_UINT:
      return "an 'unsigned integer";
    case TRAIT_TYPE_STRING:
      return "a 'string";
    default:
      return "any data type";
  }
}
