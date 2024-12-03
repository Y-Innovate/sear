#ifndef __RACFU_KEY_MAP_H_
#define __RACFU_KEY_MAP_H_

#include <cstdint>
#include <nlohmann/json.hpp>

#include "key_map_structs.hpp"
#include "key_map_user.hpp"

const key_mapping_t KEY_MAP[] = {
    {"user", segment_count(USER_SEGMENT_KEY_MAP), USER_SEGMENT_KEY_MAP}
};

const char *get_racfu_key(const char *profile_type, const char *segment,
                          const char *racf_key);

const char *get_racf_key(const char *profile_type, const char *segment,
                         const char *racfu_key, int8_t trait_type,
                         int8_t trait_operator);

const char get_racfu_trait_type(const char *profile_type, const char *segment,
                                const char *racf_key);

const char get_racf_trait_type(const char *profile_type, const char *segment,
                               const char *racfu_key);

int8_t map_operator(std::string trait_operator);
int8_t map_trait_type(const nlohmann::json &trait);

#endif
