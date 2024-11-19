#include "parameter_validation.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#include "errors.hpp"

void validate_parameters(nlohmann::json* request, nlohmann::json* errors,
                         std::string* operation, std::string* admin_type,
                         std::string* profile_name, std::string* class_name) {
  // Obtain JSON Parameter information and Validate as appropriate
  nlohmann::json checked_parameters = nlohmann::json::array();

  // Json arrays of valid values for different parameters
  nlohmann::json yes_or_no{"yes", "no"};
  nlohmann::json yes_no_or_force{"yes", "no", "force"};
  nlohmann::json valid_operations{"add", "alter", "extract", "delete"};
  nlohmann::json valid_setropts_operations{"alter", "extract"};
  nlohmann::json valid_no_add_operations{"alter", "extract", "delete"};
  nlohmann::json valid_extract_admin_types{
      "user", "group", "group-connection", "resource", "data-set", "setropts"};
  nlohmann::json valid_non_extract_admin_types{
      "user",     "group",    "group-connection", "resource",
      "data-set", "setropts", "permission"};
  nlohmann::json no_validation = nlohmann::json::object();

  // Validate parameters that are always necessary first
  if (validate_parameter(request, errors, "operation", &valid_operations,
                         *admin_type, true) == 0) {
    *admin_type = (*request)["admin_type"].get<std::string>();
  }
  if (validate_parameter(request, errors, "admin_type",
                         &valid_non_extract_admin_types, *admin_type,
                         true) == 0) {
    *operation = (*request)["operation"].get<std::string>();
  }
  if (!(errors)->empty()) {
    // Not enough information to validate other parameters
    return;
  }
  checked_parameters.push_back("operation");
  if (*operation == "extract") {
    // Call will go to IRRSEQ00 for EXTRACT operations
    validate_parameter(request, errors, "admin_type",
                       &valid_extract_admin_types, *admin_type, true);
    if (*admin_type != "setropts") {
      if (validate_parameter(request, errors, "profile_name", &no_validation,
                             *admin_type, true) == 0) {
        *profile_name = (*request)["profile_name"].get<std::string>();
      }
      checked_parameters.push_back("profile_name");
      if (*admin_type == "resource") {
        if (validate_parameter(request, errors, "class_name", &no_validation,
                               *admin_type, true) == 0) {
          *class_name = (*request)["class_name"].get<std::string>();
        }
        checked_parameters.push_back("class_name");
      }
    }
    validate_supplemental_parameters(request, errors, &checked_parameters,
                                     false);
    return;
  }
  // Call will go to IRRSMO00 for NON-EXTRACT operations
  // Each operation has required or allowed parameters, these checks are made in
  // order to validate that every required parameter is specified and optional
  // parameters are only used for supported operations and admin types
  validate_parameter(request, errors, "run", &yes_or_no, (*admin_type), false);
  checked_parameters.push_back("run");
  if (*admin_type == "setropts") {
    // SETROPTS only requires 'admin_type' and 'operation' and allows 'run'
    // SETROPTS only supports 'alter' and 'extract' operations
    validate_parameter(request, errors, "operation", &valid_setropts_operations,
                       *admin_type, true);
    validate_supplemental_parameters(request, errors, &checked_parameters,
                                     true);
    return;
  }
  checked_parameters.push_back("override");
  if (validate_parameter(request, errors, "profile_name", &no_validation,
                         *admin_type, true) == 0) {
    *profile_name = (*request)["profile_name"].get<std::string>();
  }
  checked_parameters.push_back("profile_name");
  if ((*admin_type == "user") || (*admin_type == "group")) {
    // USER and GROUP also allow 'override' and require 'profile_name'
    validate_supplemental_parameters(request, errors, &checked_parameters,
                                     true);
    return;
  }
  if (*admin_type == "group-connection") {
    // GROUP-CONNECTION also requires 'goup' but no other admin types do
    // GROUP-CONNECTION only supports 'alter', 'delete' and 'extract' operations
    validate_parameter(request, errors, "operation", &valid_no_add_operations,
                       *admin_type, true);
    validate_parameter(request, errors, "group", &no_validation, *admin_type,
                       true);
    checked_parameters.push_back("group");
    validate_supplemental_parameters(request, errors, &checked_parameters,
                                     true);
    return;
  }
  if (*admin_type == "permission") {
    // PERMISSION only supports 'alter', 'delete' and 'extract' operations
    validate_parameter(request, errors, "operation", &valid_no_add_operations,
                       *admin_type, true);
  }
  if ((*admin_type == "resource") || (*admin_type == "permission")) {
    // RESOURCE and PERMISSION also require 'class_name' but DATA-SET does not
    if (validate_parameter(request, errors, "class_name", &no_validation,
                           *admin_type, true) == 0) {
      *class_name = (*request)["class_name"].get<std::string>();
    } else {
      validate_supplemental_parameters(request, errors, &checked_parameters,
                                       true);
      return;
    }
    checked_parameters.push_back("class_name");
    if ((*admin_type == "resource") || (*class_name != "dataset")) {
      // RESOURCE and PERMISSION (for non DATASET class) do not support any
      // additional parameters
      validate_supplemental_parameters(request, errors, &checked_parameters,
                                       true);
      return;
    }
  }
  if ((*admin_type == "data-set") || (*admin_type == "permission")) {
    // DATA-SET and PERMISSION (for DATASET class) allow for 'volume' and
    // 'generic' parameters
    validate_parameter(request, errors, "volume", &no_validation, *admin_type,
                       false);
    checked_parameters.push_back("volume");
    validate_parameter(request, errors, "generic", &yes_or_no, *admin_type,
                       false);
    checked_parameters.push_back("generic");
    validate_supplemental_parameters(request, errors, &checked_parameters,
                                     true);
  }
}

uint8_t validate_parameter(nlohmann::json* request, nlohmann::json* errors,
                           std::string json_key, nlohmann::json* valid_values,
                           std::string admin_type, bool required) {
  if (!(*request).contains(json_key) && required && admin_type.empty()) {
    // Required Parameter for ALL Admin Types
    update_error_json(errors, REQUIRED_PARAMETER,
                      nlohmann::json{
                          {"parameter", json_key}
    });
    return REQUIRED_PARAMETER;
  }
  if (!(*request).contains(json_key) && required) {
    // Required Parameter for passed Admin Type
    update_error_json(errors, MISSING_PARAMETER,
                      nlohmann::json{
                          { "parameter",   json_key},
                          {"admin_type", admin_type}
    });
    return MISSING_PARAMETER;
  }
  if (!(*request).contains(json_key)) {
    // Parameter is not required and not present
    return 0;
  }
  if (!(*request)[json_key].is_string()) {
    // Parameter needs to be a string
    update_error_json(errors, BAD_PARAMETER_DATA_TYPE,
                      nlohmann::json{
                          {"parameter", json_key},
                          {"data_type", "string"}
    });
    return BAD_PARAMETER_DATA_TYPE;
  }
  std::string val = (*request)[json_key].get<std::string>();
  std::transform(val.begin(), val.end(), val.begin(), ::tolower);
  if (val.empty()) {
    // Parameter key is present but value is an empty string
    update_error_json(errors, BAD_PARAMETER_VALUE,
                      nlohmann::json{
                          {      "parameter", json_key},
                          {"parameter_value",      val}
    });
    return BAD_PARAMETER_VALUE;
  }
  if (valid_values->empty()) {
    return 0;  // Parameter has no validation and is present
  }
  for (const auto& item : valid_values->items()) {
    if (val.compare(item.value()) == 0) {
      return 0;  // Parameter meets validation rules
    }
  }
  // Parameter value does not meet validation rules
  update_error_json(errors, BAD_PARAMETER_VALUE,
                    nlohmann::json{
                        {      "parameter", json_key},
                        {"parameter_value",      val}
  });
  return BAD_PARAMETER_VALUE;
}

void validate_supplemental_parameters(nlohmann::json* request,
                                      nlohmann::json* errors,
                                      nlohmann::json* checked_parameters,
                                      bool traits_allowed) {
  nlohmann::json stable_parameters{"run_as_user_id", "admin_type"};
  checked_parameters->insert(checked_parameters->end(),
                             stable_parameters.begin(),
                             stable_parameters.end());

  for (const auto& item : request->items()) {
    if ((item.key() == "traits") && traits_allowed) {
      if (!(*request)["traits"].is_structured()) {
        update_error_json(errors, BAD_PARAMETER_DATA_TYPE,
                          nlohmann::json{
                              {"parameter", "traits"},
                              {"data_type",   "json"}
        });
      }
      continue;
    }
    bool found_match = false;
    for (const auto& allowed : checked_parameters->items()) {
      if (item.key().compare(allowed.value()) == 0) {
        found_match = true;
        continue;
      }
    }
    if (found_match) {
      continue;
    }

    // Anything else shouldn't be here
    update_error_json(errors, BAD_PARAMETER_NAME,
                      nlohmann::json{
                          {"parameter", item.key()}
    });
  }
  return;
}
