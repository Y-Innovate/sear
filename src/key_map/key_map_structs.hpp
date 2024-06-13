#ifndef __KEY_MAP_STRUCTS_H_
#define __KEY_MAP_STRUCTS_H_

#include <stdbool.h>

#define TRAIT_TYPE_BOOLEAN  0
#define TRAIT_TYPE_STRING   1
#define TRAIT_TYPE_INTEGER  2
#define TRAIT_TYPE_REPEAT   3

typedef struct {
  const char racfu_key[256];              // RACFu Key (i.e., 'omvs:default_shell') 
  const char racf_key[8+1];               // RACF Key (i.e., 'program')
  const char data_type;                   // Data Type (i.e., TRAIT_TYPE_BOOLEAN)
} trait_key_mapping_t;

typedef struct {
  const char segment[8+1];                // The name of the segment. 
  const int size;                         // The number of fields in the segment.
  const trait_key_mapping_t *traits;      // A pointer to the array of trait key
} segment_key_mapping_t;                  // mappings for this segment.

typedef struct {
  const char profile_type[8+1];           // The type of profile (i.e., 'user')
  const int size;                         // The number of segments in the profile.
  const segment_key_mapping_t *segments;
} key_mapping_t;

#define field_count(segment) sizeof(segment)/sizeof(trait_key_mapping_t)
#define segment_count(profile) sizeof(profile)/sizeof(segment_key_mapping_t)

#endif