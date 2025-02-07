#ifndef __RACFU_TRAIT_VALIDATION_H_
#define __RACFU_TRAIT_VALIDATION_H_

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

void validate_traits(std::string adminType, nlohmann::json *traits_p,
                     nlohmann::json *errors_p);
void validate_json_value_to_string(const nlohmann::json &trait,
                                   char expected_type,
                                   nlohmann::json *errors_p);

#endif
