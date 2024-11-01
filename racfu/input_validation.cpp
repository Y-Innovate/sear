#include "input_validation.hpp"

#include <stdint.h>

#include <nlohmann/json.hpp>
#include <string>

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
    update_error_json(errors, "missing_header_attribute", "operation");
    operation = "";
  } else {
    operation = request["operation"].get<std::string>();
  }
  if (!request.contains("admin_type")) {
    update_error_json(errors, "missing_header_attribute", "admin_type");
    admin_type = "";
  } else {
    admin_type = request["admin_type"].get<std::string>();
  }
  if (!(errors)->empty()) {
    // Not enough information to validate other headers
    return;
  }
  remove_header_and_validate(&request, errors, "operation", valid_operations,
                             true);
  if (operation.compare("extract") == 0) {
    // Call will go to IRRSEQ00
    remove_header_and_validate(&request, errors, "admin_type",
                               valid_extract_admin_types, true);
    if (admin_type.compare("setropts") != 0) {
      remove_header_and_validate(&request, errors, "profile_name",
                                 no_validation, true);
      if (admin_type.compare("resource") == 0) {
        remove_header_and_validate(&request, errors, "class_name",
                                   no_validation, true);
      }
    }
    validate_remaining_request_headers(request, errors, false);
    return;
  }

  remove_header_and_validate(&request, errors, "run", yes_or_no, false);
  if (admin_type.compare("setropts") == 0) {
    validate_remaining_request_headers(request, errors, true);
    return;
  }
  remove_header_and_validate(&request, errors, "override", yes_no_or_force,
                             false);
  remove_header_and_validate(&request, errors, "profile_name", no_validation,
                             true);
  if ((admin_type.compare("user") == 0) || (admin_type.compare("group") == 0)) {
    validate_remaining_request_headers(request, errors, true);
    return;
  }
  if (admin_type.compare("group-connection") == 0) {
    remove_header_and_validate(&request, errors, "group", no_validation, true);
    validate_remaining_request_headers(request, errors, true);
    return;
  }
  if ((admin_type.compare("resource") == 0) ||
      (admin_type.compare("permission") == 0)) {
    remove_header_and_validate(&request, errors, "class_name", no_validation,
                               true);
    if (admin_type.compare("resource") == 0 ||
        (className.compare("data-set") != 0)) {
      validate_remaining_request_headers(request, errors, true);
      return;
    }
  }
  if ((admin_type.compare("data-set") == 0) ||
      (admin_type.compare("permission") == 0)) {
    remove_header_and_validate(&request, errors, "volume", no_validation,
                               false);
    remove_header_and_validate(&request, errors, "generic", yes_or_no, false);
    validate_remaining_request_headers(request, errors, true);
    return;
  }
  // If we did not return yet, admin_type was not valid
  update_error_json(errors, "bad_header_value", "admin_type:" + admin_type);
  return;
}

void remove_header_and_validate(nlohmann::json* request, nlohmann::json* errors,
                                std::string json_key, nlohmann::json validation,
                                bool required) {
  std::string val;
  if ((*request).contains(json_key)) {
    // May need to try and error out if value not string
    val = (*request)[json_key].get<std::string>();
    std::transform(val.begin(), val.end(), val.begin(), ::tolower);
    (*request).erase(json_key);
    if (validation.empty()) {
      if (val.empty()) {
        update_error_json(errors, "bad_header_value", json_key + ":" + val);
        return;
      }

    } else {
      for (auto& item : validation.items()) {
        if (val.compare(item.value()) == 0) {
          return;
        }
      }
      update_error_json(errors, "bad_header_value", json_key + ":" + val);
    }
  } else {
    if (required) {
      update_error_json(errors, "missing_header_attribute", json_key);
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
    update_error_json(errors, "bad_header_name", item.key());
  }
  return;
}

void update_error_json(nlohmann::json* errors, std::string error_type,
                       std::string error_data) {
  if ((*errors).empty()) {
    (*errors)["errors"] = {};
  }
  if ((*errors)["errors"].contains(error_type)) {
    (*errors)["errors"][error_type].push_back(error_data);
  } else {
    (*errors)["errors"][error_type] = {error_data};
  }
}

nlohmann::json format_error_json(nlohmann::json errors) {
  std::string error_message_str;
  nlohmann::json output = {};
  for (auto& error_type : errors.items()) {
    for (auto& error_focus : error_type.value().items()) {
      error_message_str = "RACFu encountered a " + error_type.key() +
                          " error while working with " +
                          error_focus.value().get<std::string>() +
                          ". If you supplied this as part of your input "
                          "json, you may need to re-examine this item.";
      output += error_message_str;
    }
  }
  return output;
}