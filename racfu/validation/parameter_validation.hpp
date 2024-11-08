#ifndef __RACFU_PARAMETER_VALIDATION_H_
#define __RACFU_PARAMETER_VALIDATION_H_

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

void validate_parameters(nlohmann::json* request, nlohmann::json* errors,
                         std::string* operation, std::string* admin_type,
                         std::string* profile_name, std::string* class_name);

uint8_t validate_parameter(nlohmann::json* request, nlohmann::json* errors,
                           std::string json_key, nlohmann::json* valid_values,
                           std::string admin_type, bool required);

void validate_supplemental_parameters(nlohmann::json* request,
                                      nlohmann::json* errors,
                                      nlohmann::json* checked_parameters,
                                      bool traits_allowed);

#endif
