#include "key_map.hpp"

#include <stdio.h>
#include <string.h>

static const trait_key_mapping_t *get_key_mapping(
    const char *profile_type, // The profile type (i.e., 'user')
    const char *segment,      // The segment      (i.e., 'omvs')
    const char *racf_key,     // The RACF key     (i.e., 'program')
    const char *racfu_key,    // The RACFu key    (i.e., 'omvs:default_shell')
    bool extract);            // Set to 'true' to get the RACFu Key
                              // Set to 'false' to get the RACF Key

const char *get_racfu_key(
    const char *profile_type, 
    const char *segment,
    const char *racf_key
) {
  const trait_key_mapping_t *key_mapping = 
    get_key_mapping(profile_type, segment, racf_key, NULL, true);
  if (key_mapping == NULL) { return NULL; }
  return key_mapping->racfu_key;
}

const char *get_racf_key(
    const char *profile_type, 
    const char *segment,
    const char *racfu_key
) {
  const trait_key_mapping_t *key_mapping = 
    get_key_mapping(profile_type, segment, NULL, racfu_key, false);
  if (key_mapping == NULL) { return NULL; }
  return key_mapping->racf_key;
}

const char get_racfu_trait_type(
    const char *profile_type,
    const char *segment,
    const char *racf_key
) {
  const trait_key_mapping_t *key_mapping = 
    get_key_mapping(profile_type, segment, racf_key, NULL, true);
  if (key_mapping == NULL) { return -1; }
  return key_mapping->data_type;
}

const char get_racf_trait_type(
    const char *profile_type,
    const char *segment,
    const char *racfu_key
) {
  const trait_key_mapping_t *key_mapping = 
    get_key_mapping(profile_type, segment, NULL, racfu_key, true);
  if (key_mapping == NULL) { return -1; }
  return key_mapping->data_type;
}

static const trait_key_mapping_t *get_key_mapping(
    const char *profile_type,
    const char *segment,
    const char *racf_key,
    const char *racfu_key,
    bool extract
) {
  // Search for segment key mappings for the provided profile type
  for (int i = 0; i < sizeof(KEY_MAP)/sizeof(segment_key_mapping_t); i++) {
    if (strcmp(profile_type, KEY_MAP[i].profile_type) == 0) {
      // Find the trait key mappings for the provided segment
      for (int j = 0; j < KEY_MAP[i].size; j++) {
        if (strcmp(segment, KEY_MAP[i].segments[j].segment) == 0) {
          // Find the trait key mapping.
          for (int k = 0; k < KEY_MAP[i].segments[j].size; k++) {
            // Get the RACFu key mapping for profile extract
            if (extract == true) {
              if (strcmp(racf_key, KEY_MAP[i].segments[j].traits[k].racf_key) == 0) {
                return &KEY_MAP[i].segments[j].traits[k];
              }
            }
            // Get the RACF key mapping for add/alter/delete
            else {
              if (strcmp(racfu_key, KEY_MAP[i].segments[j].traits[k].racfu_key) == 0) {
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