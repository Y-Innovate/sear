#include "error_validation.hpp"

#include <stdint.h>
#include <stdio.h>

#include <nlohmann/json.hpp>
#include <string>

#include "key_map.hpp"

void validate_parameters(nlohmann::json request, nlohmann::json* errors) {
  // Obtain JSON Header information and Validate as appropriate

  std::string operation = "", admin_type = "", className;
  nlohmann::json yes_or_no = {"yes", "no"};
  nlohmann::json yes_no_or_force = {"yes", "no", "force"};
  nlohmann::json valid_operations = {"add", "alter", "extract", "delete"};
  nlohmann::json valid_extract_admin_types = {
      "user", "group", "group-connection", "resource", "data-set", "setropts"};
  nlohmann::json no_validation = {};

  validate_parameter(&request, errors, "operation", valid_operations,
                     admin_type, true, false);
  validate_parameter(&request, errors, "admin_type", no_validation, admin_type,
                     true, false);

  if (!(errors)->empty()) {
    // Not enough information to validate other parameters
    return;
  }
  printf("%s\n", request.dump().c_str());
  admin_type = request["admin_type"].get<std::string>();
  operation = request["operation"].get<std::string>();
  request.erase("operation");
  if (operation.compare("extract") == 0) {
    // Call will go to IRRSEQ00
    validate_parameter(&request, errors, "admin_type",
                       valid_extract_admin_types, admin_type, true, true);
    if (admin_type.compare("setropts") != 0) {
      validate_parameter(&request, errors, "profile_name", no_validation,
                         admin_type, true, true);
      if (admin_type.compare("resource") == 0) {
        validate_parameter(&request, errors, "class_name", no_validation,
                           admin_type, true, true);
      }
    }
    validate_remaining_request_attributes(request, errors, false);
    return;
  }
  validate_parameter(&request, errors, "run", yes_or_no, admin_type, false,
                     true);
  if (admin_type.compare("setropts") == 0) {
    validate_remaining_request_attributes(request, errors, true);
    return;
  }
  validate_parameter(&request, errors, "override", yes_no_or_force, admin_type,
                     false, true);
  validate_parameter(&request, errors, "profile_name", no_validation,
                     admin_type, true, true);
  if ((admin_type.compare("user") == 0) || (admin_type.compare("group") == 0)) {
    validate_remaining_request_attributes(request, errors, true);
    return;
  }
  if (admin_type.compare("group-connection") == 0) {
    validate_parameter(&request, errors, "group", no_validation, admin_type,
                       true, true);
    validate_remaining_request_attributes(request, errors, true);
    return;
  }
  if ((admin_type.compare("resource") == 0) ||
      (admin_type.compare("permission") == 0)) {
    validate_parameter(&request, errors, "class_name", no_validation,
                       admin_type, true, true);
    if (admin_type.compare("resource") == 0 ||
        (className.compare("data-set") != 0)) {
      validate_remaining_request_attributes(request, errors, true);
      return;
    }
  }
  if ((admin_type.compare("data-set") == 0) ||
      (admin_type.compare("permission") == 0)) {
    validate_parameter(&request, errors, "volume", no_validation, admin_type,
                       false, true);
    validate_parameter(&request, errors, "generic", yes_or_no, admin_type,
                       false, true);
    validate_remaining_request_attributes(request, errors, true);
    return;
  }
  update_error_json(
      errors, BAD_PARAMETER_VALUE,
      {
          {      "parameter", "admin_type"},
          {"parameter_value",   admin_type}
  });
  return;
}

void validate_parameter(nlohmann::json* request, nlohmann::json* errors,
                        std::string json_key, nlohmann::json validation,
                        std::string admin_type, bool required, bool remove) {
  std::string val;
  if ((*request).contains(json_key)) {
    if (!(*request)[json_key].is_string()) {
      update_error_json(errors, BAD_PARAMETER_DATA_TYPE,
                        {
                            {"parameter", json_key}
      });
      return;
    }
    val = (*request)[json_key].get<std::string>();
    std::transform(val.begin(), val.end(), val.begin(), ::tolower);
    if (remove) {
      (*request).erase(json_key);
    }
    if (validation.empty()) {
      if (val.empty()) {
        update_error_json(
            errors, BAD_PARAMETER_VALUE,
            {
                {      "parameter", json_key},
                {"parameter_value",      val}
        });
        return;
      }

    } else {
      for (auto& item : validation.items()) {
        if (val.compare(item.value()) == 0) {
          return;
        }
      }
      update_error_json(errors, BAD_PARAMETER_VALUE,
                        {
                            {      "parameter", json_key},
                            {"parameter_value",      val}
      });
    }
  } else {
    if (required) {
      if (admin_type.empty()) {
        update_error_json(errors, REQUIRED_PARAMETER,
                          {
                              {"parameter", json_key}
        });
      } else {
        update_error_json(
            errors, MISSING_PARAMETER,
            {
                { "parameter",   json_key},
                {"admin_type", admin_type}
        });
      }
    }
  }
}

void validate_remaining_request_attributes(nlohmann::json request,
                                           nlohmann::json* errors,
                                           bool traits_allowed) {
  nlohmann::json supplemental_parameters = {
      "running_user_id", "result_buffer_size", "debug_mode", "admin_type"};
  for (auto& item : request.items()) {
    // Traits contain no Header information and is ignored
    if ((item.key().compare("traits") == 0) && traits_allowed) {
      continue;
    } else {
      bool found_match = false;
      for (auto& allowed : supplemental_parameters.items()) {
        if (item.key().compare(allowed.value()) == 0) {
          found_match = true;
          continue;
        }
      }
      if (found_match) {
        continue;
      }
    }
    // Anything else shouldn't be here
    update_error_json(errors, BAD_PARAMETER_NAME,
                      {
                          {"parameter", item.key()}
    });
  }
  return;
}

void update_error_json(nlohmann::json* errors, int8_t error_type,
                       nlohmann::json error_data) {
  nlohmann::json error_json = {
      {"error_code", error_type},
      {"error_data", error_data}
  };
  if (error_type == XML_PARSE_ERROR) {
    (*errors).push_back(error_json);
    return;
  }
  if ((*errors).empty()) {
    (*errors)["errors"] = {};
  }
  (*errors)["errors"].push_back(error_json);
}

nlohmann::json format_error_json(nlohmann::json errors) {
  std::string error_message_str;
  nlohmann::json error_data, output = {
                                 {"errors", {}}
  };
  // printf("%s\n", errors.dump().c_str());
  for (int i = 0; i < errors.size(); i++) {
    // printf("%s\n", errors[i].dump().c_str());
    if (!errors[i]["error_data"].empty()) {
      error_data = errors[i]["error_data"];
      // printf("%s\n", error_data.dump().c_str());
    }
    switch (errors[i]["error_code"].get<uint>()) {
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
                            "' must be a string value";
        break;
      case BAD_PARAMETER_DATA_TYPE:
        error_message_str = "'" + error_data["parameter"].get<std::string>() +
                            "' is not a valid parameter";
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
            "'" + error_data["trait"].get<std::string>() + "' must use a '" +
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
        error_message_str = "could not parse XML from IRRSMO00";
        break;
      default:
        error_message_str = "An unknown error has occurred";
    }
    output["errors"] += error_message_str;
  }
  // printf("%s\n", output.dump().c_str());
  return output;
}

std::string decode_data_type(uint data_type_code) {
  switch (data_type_code) {
    case TRAIT_TYPE_BOOLEAN:
      return "boolean";
    case TRAIT_TYPE_UINT:
      return "unsigned";
    case TRAIT_TYPE_STRING:
      return "string";
    default:
      return "any data type";
  }
}