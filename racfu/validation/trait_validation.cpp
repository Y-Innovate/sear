#include "trait_validation.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>

#include "errors.hpp"
#include "key_map.hpp"

void validate_traits(std::string adminType, nlohmann::json* traits,
                     nlohmann::json* errors) {
  // Parses the json for the traits (segment-trait information) passed in a
  // json object and validates the structure, format and types of this data
  std::string currentSegment = "", itemSegment, itemTrait, itemOperation;
  const char* translatedKey;

  std::regex segment_trait_key_regex{R"~((([a-z]*):*)([a-z]*):(.*))~"};
  std::smatch segment_trait_key_data;

  for (const auto& item : traits->items()) {
    if (!regex_match(item.key(), segment_trait_key_data,
                     segment_trait_key_regex)) {
      // Track any entries that do not match proper syntax
      update_error_json(errors, BAD_TRAIT_STRUCTURE,
                        nlohmann::json{
                            {"trait", item.key()}
      });
      continue;
    }
    if (segment_trait_key_data[3] == "") {
      itemOperation = "";
      itemSegment = segment_trait_key_data[2];
    } else {
      itemOperation = segment_trait_key_data[2];
      itemSegment = segment_trait_key_data[3];
    }
    itemTrait = segment_trait_key_data[4];

    // Get passed operation and validate it
    int8_t operation = map_operations(itemOperation);
    if (operation == OPERATOR_BAD) {
      update_error_json(errors, BAD_OPERATION,
                        nlohmann::json{
                            {"operation", itemOperation}
      });
      continue;
    }
    int8_t trait_type = map_trait_type(item.value());
    int8_t expected_type =
        get_racf_trait_type(adminType.c_str(), itemSegment.c_str(),
                            (itemSegment + ":" + itemTrait).c_str());
    // Validate Segment-Trait by ensuring a TRAIT_TYPE is found
    if (expected_type == TRAIT_TYPE_BAD) {
      update_error_json(errors, BAD_SEGMENT_TRAIT_COMBO,
                        nlohmann::json{
                            {"segment", itemSegment},
                            {  "trait",   itemTrait}
      });
      continue;
    }
    // Validate the type of the passed data matches the expected TRAIT_TYPE
    if (trait_type != expected_type) {
      update_error_json(
          errors, BAD_TRAIT_DATA_TYPE,
          nlohmann::json{
              {        "trait",    item.key()},
              {"required_type", expected_type}
      });
      continue;
    }
    translatedKey = get_racf_key(adminType.c_str(), itemSegment.c_str(),
                                 (itemSegment + ":" + itemTrait).c_str(),
                                 trait_type, operation);
    // If we could not find the RACF key with this function, the operation is
    // bad because we check the Segment-Trait combination above
    if (translatedKey == NULL) {
      update_error_json(errors, BAD_TRAIT_OPERATION_COMBO,
                        nlohmann::json{
                            {"operation", itemOperation},
                            {  "segment",   itemSegment},
                            {    "trait",     itemTrait}
      });
    }
    // Passed all of our validation so we go around the loop again
  }
}

int8_t map_operations(std::string operation) {
  if (operation.empty()) {
    return OPERATOR_ANY;
  }
  std::transform(operation.begin(), operation.end(), operation.begin(),
                 ::tolower);
  if (operation == "set") {
    return OPERATOR_SET;
  }
  if (operation == "add") {
    return OPERATOR_ADD;
  }
  if (operation == "remove") {
    return OPERATOR_REMOVE;
  }
  if (operation == "delete") {
    return OPERATOR_DELETE;
  }
  return OPERATOR_BAD;
}

int8_t map_trait_type(const nlohmann::json& trait) {
  if (trait.is_boolean() || trait.is_null()) {
    return TRAIT_TYPE_BOOLEAN;
  }
  if (trait.is_string() || trait.is_array()) {
    return TRAIT_TYPE_STRING;
  }
  if (trait.is_number_unsigned()) {
    return TRAIT_TYPE_UINT;
  }
  if (trait.is_object() || trait.is_number()) {
    return TRAIT_TYPE_BAD;
  }
  return TRAIT_TYPE_ANY;
}

std::string json_value_to_string(const nlohmann::json& trait,
                                 char expected_type, nlohmann::json* errors) {
  if (trait.is_string()) {
    return trait.get<std::string>();
  }
  if (trait.is_array()) {
    std::string output_string = "";
    std::string delimeter =
        ", ";  // May just be " " or just be ","; May need to test
    for (const auto& item : trait.items()) {
      if (!item.value().is_string()) {
        update_error_json(
            errors, BAD_TRAIT_DATA_TYPE,
            nlohmann::json{
                {        "trait",    item.key()},
                {"required_type", expected_type}
        });
        return trait.dump();
      }
      output_string += item.value().get<std::string>() + delimeter;
    }
    for (int i = 0; i < delimeter.length(); i++) {
      output_string.pop_back();
    }
    return output_string;
  }
  return trait.dump();
}
