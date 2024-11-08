#include "errors.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#include "key_map.hpp"

void update_error_json(nlohmann::json* errors, int8_t error_type,
                       nlohmann::json error_data) {
  nlohmann::json error_json = {
      {"error_code", error_type},
      {"error_data", error_data}
  };

  if (error_type >= XML_PARSE_ERROR) {
    errors->push_back(error_json);
    return;
  }
  if ((*errors).empty()) {
    (*errors)["errors"] = nlohmann::json::array();
  }
  (*errors)["errors"].push_back(error_json);
}

nlohmann::json format_error_json(nlohmann::json* errors) {
  std::string error_message_str;
  nlohmann::json error_data, output = {
                                 {"errors", {}}
  };
  for (int i = 0; i < errors->size(); i++) {
    if (!(*errors)[i]["error_data"].empty()) {
      error_data = (*errors)[i]["error_data"];
    }
    switch ((*errors)[i]["error_code"].get<uint>()) {
      case BAD_PARAMETER_VALUE:
        error_message_str = "'" +
                            error_data["parameter_value"].get<std::string>() +
                            "' is not a valid value for '" +
                            error_data["parameter"].get<std::string>() + "'";
        break;
      case REQUIRED_PARAMETER:
        error_message_str = "'" + error_data["parameter"].get<std::string>() +
                            "' is a required parameter";
        break;
      case MISSING_PARAMETER:
        error_message_str = "'" + error_data["parameter"].get<std::string>() +
                            "' is a required parameter for the '" +
                            error_data["admin_type"].get<std::string>() +
                            "' 'admin_type'";
        break;
      case BAD_PARAMETER_NAME:
        error_message_str = "'" + error_data["parameter"].get<std::string>() +
                            "' is not a valid parameter";
        break;
      case BAD_PARAMETER_DATA_TYPE:
        error_message_str =
            "'" + error_data["parameter"].get<std::string>() + "' must be a " +
            error_data["data_type"].get<std::string>() + " value";
        break;
      case BAD_TRAIT_STRUCTURE:
        error_message_str = "'" + error_data["trait"].get<std::string>() +
                            "' is not in '<segment>:<trait>' or "
                            "'<operation>:<segment>:<trait>' format";
        break;
      case BAD_OPERATION:
        error_message_str = "'" + error_data["operation"].get<std::string>() +
                            "' is not a valid trait operation";
        break;
      case BAD_TRAIT_DATA_TYPE:
        error_message_str =
            "'" + error_data["trait"].get<std::string>() + "' must be " +
            decode_data_type(error_data["required_type"].get<uint>()) +
            "' value";
        break;
      case BAD_SEGMENT_TRAIT_COMBO:
        error_message_str = "'" + error_data["segment"].get<std::string>() +
                            ":" + error_data["trait"].get<std::string>() +
                            "' is not a valid trait";
        break;
      case BAD_TRAIT_OPERATION_COMBO:
        error_message_str = "'" + error_data["operation"].get<std::string>() +
                            "' is not a valid operation for '" +
                            error_data["segment"].get<std::string>() + ":" +
                            error_data["trait"].get<std::string>() + "'";
        break;
      case XML_PARSE_ERROR:
        error_message_str = "could not parse XML returned from IRRSMO00";
        break;
      default:
        error_message_str = "An unknown error has occurred";
    }
    output["errors"] += error_message_str;
  }
  return output;
}

std::string decode_data_type(uint data_type_code) {
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
