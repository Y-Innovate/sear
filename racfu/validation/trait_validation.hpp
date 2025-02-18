#ifndef __RACFU_TRAIT_VALIDATION_H_
#define __RACFU_TRAIT_VALIDATION_H_

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#include "errors.hpp"

void validate_traits(std::string adminType, nlohmann::json* traits_p,
                     RACFu::Errors& errors);
void validate_json_value_to_string(const nlohmann::json& trait,
                                   char expected_type, RACFu::Errors& errors);

#endif
