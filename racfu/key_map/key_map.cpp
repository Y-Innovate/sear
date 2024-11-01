#include "key_map.hpp"

#include <stdio.h>
#include <string.h>

static const trait_key_mapping_t *get_key_mapping(
    const char *profile_type,  // The profile type (i.e., 'user')
    const char *segment,       // The segment      (i.e., 'omvs')
    const char *racf_key,      // The RACF key     (i.e., 'program')
    const char *racfu_key,     // The RACFu key    (i.e., 'omvs:default_shell')
    int8_t trait_type,      // The trait type   (i.e.,  'TRAIT_TYPE_UNSIGNED')
    int8_t trait_operator,  // The operator     (i.e.,  'OPERATOR_SET')
    bool extract);          // Set to 'true' to get the RACFu Key
                            // Set to 'false' to get the RACF Key

static bool check_trait_type(int8_t actual, int8_t expected);

static bool check_trait_operator(int8_t trait_operator,
                                 const operators_allowed_t *operators_allowed);

const char *get_racfu_key(const char *profile_type, const char *segment,
                          const char *racf_key) {
  const trait_key_mapping_t *key_mapping =
      get_key_mapping(profile_type, segment, racf_key, NULL, TRAIT_TYPE_ANY,
                      OPERATOR_ANY, true);
  if (key_mapping == NULL) {
    return NULL;
  }
  return key_mapping->racfu_key;
}

const char *get_racf_key(const char *profile_type, const char *segment,
                         const char *racfu_key, int8_t trait_type,
                         int8_t trait_operator) {
  const trait_key_mapping_t *key_mapping =
      get_key_mapping(profile_type, segment, NULL, racfu_key, trait_type,
                      trait_operator, false);
  if (key_mapping == NULL) {
    return NULL;
  }
  return key_mapping->racf_key;
}

const char get_racfu_trait_type(const char *profile_type, const char *segment,
                                const char *racf_key) {
  const trait_key_mapping_t *key_mapping =
      get_key_mapping(profile_type, segment, racf_key, NULL, TRAIT_TYPE_ANY,
                      OPERATOR_ANY, true);
  if (key_mapping == NULL) {
    return TRAIT_TYPE_BAD;
  }
  return key_mapping->trait_type;
}

const char get_racf_trait_type(const char *profile_type, const char *segment,
                               const char *racfu_key) {
  const trait_key_mapping_t *key_mapping =
      get_key_mapping(profile_type, segment, NULL, racfu_key, TRAIT_TYPE_ANY,
                      OPERATOR_ANY, true);
  if (key_mapping == NULL) {
    return TRAIT_TYPE_BAD;
  }
  return key_mapping->trait_type;
}

static const trait_key_mapping_t *get_key_mapping(
    const char *profile_type, const char *segment, const char *racf_key,
    const char *racfu_key, int8_t trait_type, int8_t trait_operator,
    bool extract) {
  bool trait_type_good;
  bool trait_operator_good;
  // Search for segment key mappings for the provided profile type
  for (int i = 0; i < sizeof(KEY_MAP) / sizeof(segment_key_mapping_t); i++) {
    if (strcmp(profile_type, KEY_MAP[i].profile_type) == 0) {
      // Find the trait key mappings for the provided segment
      for (int j = 0; j < KEY_MAP[i].size; j++) {
        if (strcmp(segment, KEY_MAP[i].segments[j].segment) == 0) {
          // Find the trait key mapping.
          for (int k = 0; k < KEY_MAP[i].segments[j].size; k++) {
            // Get the RACFu key mapping for profile extract
            if (extract == true) {
              if (strcmp(racf_key, KEY_MAP[i].segments[j].traits[k].racf_key) ==
                  0) {
                return &KEY_MAP[i].segments[j].traits[k];
              }
            }
            // Get the RACF key mapping for add/alter/delete
            else {
              if (strcmp(racfu_key,
                         KEY_MAP[i].segments[j].traits[k].racfu_key) == 0) {
                // Check trait type
                trait_type_good = check_trait_type(
                    trait_type, KEY_MAP[i].segments[j].traits[k].trait_type);
                if (trait_type_good == false) {
                  return NULL;
                }
                // Check trait operator
                trait_operator_good = check_trait_operator(
                    trait_operator,
                    &KEY_MAP[i].segments[j].traits[k].operators_allowed);
                if (trait_operator_good == false) {
                  return NULL;
                }
                return &KEY_MAP[i].segments[j].traits[k];
              }
            }
          }
        }
      }
    }
  }
  return NULL;
}

static bool check_trait_type(int8_t actual, int8_t expected) {
  if (actual == TRAIT_TYPE_ANY) {
    return true;
  }
  if (actual != expected) {
    return false;
  }
  return true;
}

static bool check_trait_operator(int8_t trait_operator,
                                 const operators_allowed_t *operators_allowed) {
  switch (trait_operator) {
    case OPERATOR_ANY:
      return true;
    case OPERATOR_SET:
      return operators_allowed->set_allowed;
    case OPERATOR_ADD:
      return operators_allowed->add_allowed;
    case OPERATOR_REMOVE:
      return operators_allowed->remove_allowed;
    case OPERATOR_DELETE:
      return operators_allowed->delete_allowed;
    default:
      return false;
  }
}
