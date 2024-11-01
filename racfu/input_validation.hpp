#include <stdint.h>

#include <nlohmann/json.hpp>
#include <string>

#include "racfu.h"

void validate_header_attributes(nlohmann::json request, nlohmann::json* errors);

void remove_header_and_validate(nlohmann::json* request, nlohmann::json* errors,
                                std::string json_key, nlohmann::json validation,
                                bool required);

void validate_remaining_request_headers(nlohmann::json request,
                                        nlohmann::json* errors,
                                        bool traits_allowed);

void update_error_json(nlohmann::json* errors, std::string error_type,
                       std::string error_data);

nlohmann::json format_error_json(nlohmann::json errors);