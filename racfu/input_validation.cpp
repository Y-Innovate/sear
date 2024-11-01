#include "input_validation.hpp"

#include <stdint.h>

#include <nlohmann/json.hpp>
#include <string>

#include "key_map.hpp"

void validate_header_attributes(nlohmann::json request,
                                nlohmann::json* errors) {
  // Obtain JSON Header information and Validate as appropriate

  std::string requestOperation, run, override_str, profileName, group,
      className, generic, volume;
  std::string operation, admin_type;
  nlohmann::json yes_or_no = {"yes", "no"};
  nlohmann::json yes_no_or_force = {"yes", "no", "force"};
  nlohmann::json valid_operations = {"add", "alter", "extract", "delete"};
  nlohmann::json valid_extract_admin_types = {
      "user", "group", "group-connection", "resource", "data-set", "setropts"};
  nlohmann::json no_validation = {};

  if (!request.contains("operation")) {
    update_error_json(errors, REQUIRED_HEADER_ATTRIBUTE,
                      {"header_attribute", "operation"});
    operation = "";
  } else {
    operation = request["operation"].get<std::string>();
  }
  if (!request.contains("admin_type")) {
    update_error_json(errors, REQUIRED_HEADER_ATTRIBUTE,
                      {"header_attribute", "admin_type"});
    admin_type = "";
  } else {
    admin_type = request["admin_type"].get<std::string>();
  }
  if (!(errors)->empty()) {
    // Not enough information to validate other headers
    return;
  }
  remove_header_and_validate(&request, errors, "operation", valid_operations,
                             admin_type, true);
  if (operation.compare("extract") == 0) {
    // Call will go to IRRSEQ00
    remove_header_and_validate(&request, errors, "admin_type",
                               valid_extract_admin_types, admin_type, true);
    if (admin_type.compare("setropts") != 0) {
      remove_header_and_validate(&request, errors, "profile_name",
                                 no_validation, admin_type, true);
      if (admin_type.compare("resource") == 0) {
        remove_header_and_validate(&request, errors, "class_name",
                                   no_validation, admin_type, true);
      }
    }
    validate_remaining_request_headers(request, errors, false);
    return;
  }

  remove_header_and_validate(&request, errors, "run", yes_or_no, admin_type,
                             false);
  if (admin_type.compare("setropts") == 0) {
    validate_remaining_request_headers(request, errors, true);
    return;
  }
  remove_header_and_validate(&request, errors, "override", yes_no_or_force,
                             admin_type, false);
  remove_header_and_validate(&request, errors, "profile_name", no_validation,
                             admin_type, true);
  if ((admin_type.compare("user") == 0) || (admin_type.compare("group") == 0)) {
    validate_remaining_request_headers(request, errors, true);
    return;
  }
  if (admin_type.compare("group-connection") == 0) {
    remove_header_and_validate(&request, errors, "group", no_validation,
                               admin_type, true);
    validate_remaining_request_headers(request, errors, true);
    return;
  }
  if ((admin_type.compare("resource") == 0) ||
      (admin_type.compare("permission") == 0)) {
    remove_header_and_validate(&request, errors, "class_name", no_validation,
                               admin_type, true);
    if (admin_type.compare("resource") == 0 ||
        (className.compare("data-set") != 0)) {
      validate_remaining_request_headers(request, errors, true);
      return;
    }
  }
  if ((admin_type.compare("data-set") == 0) ||
      (admin_type.compare("permission") == 0)) {
    remove_header_and_validate(&request, errors, "volume", no_validation,
                               admin_type, false);
    remove_header_and_validate(&request, errors, "generic", yes_or_no,
                               admin_type, false);
    validate_remaining_request_headers(request, errors, true);
    return;
  }
  update_error_json(
      errors, BAD_HEADER_VALUE,
      {
          {"header_attribute", "admin_type"},
          {    "header_value",   admin_type}
  });
  return;
}

void remove_header_and_validate(nlohmann::json* request, nlohmann::json* errors,
                                std::string json_key, nlohmann::json validation,
                                std::string admin_type, bool required) {
  std::string val;
  if ((*request).contains(json_key)) {
    // May need to try and error out if value not string
    if (!(*request)[json_key].is_string()) {
      update_error_json(errors, BAD_HEADER_DATA_TYPE,
                        {"header_attribute", json_key});
      return;
    }
    val = (*request)[json_key].get<std::string>();
    std::transform(val.begin(), val.end(), val.begin(), ::tolower);
    (*request).erase(json_key);
    if (validation.empty()) {
      if (val.empty()) {
        update_error_json(
            errors, BAD_HEADER_VALUE,
            {
                {"header_attribute", json_key},
                {    "header_value",      val}
        });
        return;
      }

    } else {
      for (auto& item : validation.items()) {
        if (val.compare(item.value()) == 0) {
          return;
        }
      }
      update_error_json(
          errors, BAD_HEADER_VALUE,
          {
              {"header_attribute", json_key},
              {    "header_value",      val}
      });
    }
  } else {
    if (required) {
      update_error_json(
          errors, MISSING_HEADER_ATTRIBUTE,
          {
              {"header_attribute",   json_key},
              {      "admin_type", admin_type}
      });
    }
  }
}

void validate_remaining_request_headers(nlohmann::json request,
                                        nlohmann::json* errors,
                                        bool traits_allowed) {
  nlohmann::json supplemental_headers = {
      "running_user_id", "result_buffer_size", "debug_mode", "admin_type"};
  for (auto& item : request.items()) {
    // Traits contain no Header information and is ignored
    if ((item.key().compare("traits") == 0) && traits_allowed) {
      continue;
    } else {
      bool found_match = false;
      for (auto& allowed : supplemental_headers.items()) {
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
    update_error_json(errors, BAD_HEADER_NAME,
                      {"header_attribute", item.key()});
  }
  return;
}

void update_error_json(nlohmann::json* errors, int8_t error_type,
                       nlohmann::json error_data) {
  nlohmann::json error_json = {
      {"error_code", error_type},
      {"error_data", error_data}
  };
  if ((*errors).empty()) {
    (*errors)["errors"] = {};
  }
  (*errors)["errors"].push_back(error_json);
}

nlohmann::json format_error_json(nlohmann::json errors) {
  std::string error_message_str;
  nlohmann::json output = {"errors", {}}, error_data;
  for (auto& error : errors.items()) {
    error_data = error.value()["error_data"];
    switch (error.value()["error_code"].get<uint>()) {
      case BAD_HEADER_VALUE:
        error_message_str =
            "'" + error_data["header_value"].get<std::string>() +
            "' is not a valid value for '" +
            error_data["header_attribute"].get<std::string>() + "'";
        break;
      case REQUIRED_HEADER_ATTRIBUTE:
        error_message_str = "'" +
                            error_data["header_attribute"].get<std::string>() +
                            "' is a required header field";
        break;
      case MISSING_HEADER_ATTRIBUTE:
        error_message_str =
            "'" + error_data["header_attribute"].get<std::string>() +
            "' is a required field for the '" +
            error_data["admin_type"].get<std::string>() + "' 'admin_type'";
        break;
      case BAD_HEADER_NAME:
        error_message_str = "'" +
                            error_data["header_attribute"].get<std::string>() +
                            "' must be a string value";
        break;
      case BAD_HEADER_DATA_TYPE:
        error_message_str = "'" +
                            error_data["header_attribute"].get<std::string>() +
                            "' is not a valid header field";
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
      return "boolean";
    case TRAIT_TYPE_UNSIGNED:
      return "unsigned";
    case TRAIT_TYPE_STRING:
      return "string";
    default:
      return "any data type";
  }
}