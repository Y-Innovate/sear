#ifndef __RACFU_TRAIT_VALIDATION_H_
#define __RACFU_TRAIT_VALIDATION_H_

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#include "errors.hpp"

void validate_traits(const std::string& admin_type,
                     const nlohmann::json& traits, RACFu::Errors& errors);
void validate_json_value_to_string(const nlohmann::json& trait,
                                   char expected_type, RACFu::Errors& errors);

#endif
