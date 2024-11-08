#ifndef __RACFU_TRAIT_VALIDATION_H_
#define __RACFU_TRAIT_VALIDATION_H_

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

void validate_traits(std::string adminType, nlohmann::json *traits,
                     nlohmann::json *errors);
int8_t map_operations(std::string operation);
int8_t map_trait_type(const nlohmann::json &trait);
std::string json_value_to_string(const nlohmann::json &trait,
                                 char expected_type, nlohmann::json *errors);

#endif
