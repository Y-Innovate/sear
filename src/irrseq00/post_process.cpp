#include "post_process.hpp"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <iostream>

#include "key_map.hpp"

nlohmann::json post_process_generic(
    generic_extract_parms_results_t *generic_result_buffer) {
  nlohmann::json profile;
  profile["profile"] = nlohmann::json::object();
  char *profile_address = (char *)generic_result_buffer;

  // Set Class Name
  char profile_type[8];
  post_process_key(profile_type, generic_result_buffer->class_name, 8);

  // Segment Variables
  int first_segment_offset = sizeof(generic_extract_parms_results_t);
  first_segment_offset += generic_result_buffer->profile_name_length;
  generic_segment_descriptor_t *segment =
      (generic_segment_descriptor_t *)(profile_address + first_segment_offset);
  char segment_key[8];

  // Field Variables
  generic_field_descriptor_t *field;
  char field_key[8];
  std::string racfu_field_key;
  char racfu_field_type;

  // Repeat Group Variables
  std::vector<nlohmann::json> repeat_group;
  int repeat_group_count;
  int repeat_group_element_count;
  char repeat_field_key[8];
  std::string racfu_repeat_field_key;
  char racfu_repeat_field_type;

  // Post Process Segments
  for (int i = 1; i <= generic_result_buffer->segment_count; i++) {
    post_process_key(segment_key, segment->name, 8);
    profile["profile"][segment_key] = nlohmann::json::object();
    // Post Process Fields
    field = (generic_field_descriptor_t *)(profile_address +
                                           segment->field_descriptor_offset);
    for (int j = 1; j <= segment->field_count; j++) {
      racfu_field_key = post_process_field_key(field_key, profile_type,
                                               segment_key, field->name);
      racfu_field_type =
          get_racfu_trait_type(profile_type, segment_key, field_key);
      // Post Process Non-Repeat Fields
      if (!(field->type & t_repeat_field_header)) {
        process_generic_field(profile["profile"][segment_key][racfu_field_key],
                              field, field_key, profile_address,
                              racfu_field_type);
        // Post Process Repeat Fields
      } else {
        repeat_group_count =
            field->field_data_length_repeat_group_count.repeat_group_count;
        repeat_group_element_count =
            field->field_data_offset_repeat_group_element_count
                .repeat_group_element_count;
        // Post Process Each Repeat Group
        for (int k = 1; k <= repeat_group_count; k++) {
          repeat_group.push_back(nlohmann::json::object());
          // Post Process Each Repeat Group Field
          for (int l = 1; l <= repeat_group_element_count; l++) {
            field++;
            racfu_repeat_field_key = post_process_field_key(
                repeat_field_key, profile_type, segment_key, field->name);
            racfu_repeat_field_type = get_racfu_trait_type(
                profile_type, segment_key, repeat_field_key);
            process_generic_field(repeat_group[k - 1][racfu_repeat_field_key],
                                  field, repeat_field_key, profile_address,
                                  racfu_repeat_field_type);
          }
        }
        profile["profile"][segment_key][racfu_field_key] = repeat_group;
        repeat_group.clear();
      }
      field++;
    }
    segment++;
  }
  return profile;
}

nlohmann::json post_process_setropts(
    setropts_extract_results_t *setropts_result_buffer) {
  nlohmann::json profile;
  profile["profile"] = nlohmann::json::object();
  char *profile_address = (char *)setropts_result_buffer;

  // Segment Variables
  setropts_segment_descriptor_t *segment =
      (setropts_segment_descriptor_t *)(profile_address +
                                        sizeof(setropts_extract_results_t));
  char segment_key[8];

  // Field Variables
  setropts_field_descriptor_t *field =
      (setropts_field_descriptor_t *)(profile_address +
                                      sizeof(setropts_extract_results_t) +
                                      sizeof(setropts_segment_descriptor_t));
  char field_key[8];
  std::string racfu_field_key;
  char field_data[10025];  // we may want to make this dynamic using a VLA or
                           // malloc()/calloc()
  std::vector<std::string> list_field_data;
  char *list_field_data_pointer;
  char field_type;

  // Post Process Base Segment
  post_process_key(segment_key, segment->name, 8);
  profile["profile"][segment_key] = nlohmann::json::object();

  // Post Process Fields
  for (int i = 1; i <= segment->field_count; i++) {
    racfu_field_key =
        post_process_field_key(field_key, "setropts", segment_key, field->name);
    field_type = get_setropts_field_type(field_key);
    if (field->field_length != 0) {
      // Post Process List Fields
      if (field_type == SETROPTS_FIELD_TYPE_LIST) {
        list_field_data_pointer =
            (char *)field + sizeof(setropts_field_descriptor_t);
        for (int j = 0; j < field->field_length / 9; j++) {
          process_setropts_field(field_data, list_field_data_pointer, 8);
          list_field_data.push_back(field_data);
          list_field_data_pointer += 9;
        }
        profile["profile"][segment_key][racfu_field_key] = list_field_data;
        list_field_data.clear();
        // Post Process String & Number Fields
      } else {
        process_setropts_field(
            field_data, (char *)field + sizeof(setropts_field_descriptor_t),
            field->field_length);
        // Number
        if (field_type == SETROPTS_FIELD_TYPE_NUMBER) {
          profile["profile"][segment_key][racfu_field_key] =
              strtol(field_data, NULL, 10);
          // String
        } else {
          profile["profile"][segment_key][racfu_field_key] = field_data;
        }
      }
      // Post Process Boolean Fields
    } else if (field_type == SETROPTS_FIELD_TYPE_BOOLEAN) {
      if (field->flag == 0xe8) {  // 0xe8 is 'Y' in EBCDIC.
        profile["profile"][segment_key][racfu_field_key] = true;
      } else {
        profile["profile"][segment_key][racfu_field_key] = false;
      }
      // Post Process All Non-Boolean Fields Without a Value
    } else {
      profile["profile"][segment_key][racfu_field_key] = nullptr;
    }
    field =
        (setropts_field_descriptor_t *)((char *)field +
                                        sizeof(setropts_field_descriptor_t) +
                                        field->field_length);
  }
  return profile;
}

void process_generic_field(nlohmann::json &json_field,
                           generic_field_descriptor_t *field, char *field_key,
                           char *profile_address, const char racfu_field_type) {
  char field_data[1025];  // we may want to make this dynamic using a VLA or
                          // malloc()/calloc()
  // Post Process Boolean Fields
  if (field->type & t_boolean_field) {
    if (field->flags & f_boolean_field) {
      json_field = true;
    } else {
      json_field = false;
    }
    // Post Process Generic Fields
  } else {
    int field_length =
        field->field_data_length_repeat_group_count.field_data_length;
    memset(field_data, 0, field_length + 1);
    copy_and_encode_string(
        field_data,
        profile_address + field->field_data_offset_repeat_group_element_count
                              .field_data_offset,
        field_length);
    // Set Empty Fields to 'null'
    if (strcmp(field_data, "") == 0) {
      json_field = nullptr;
      // Cast Integer Fields
    } else if (racfu_field_type == TRAIT_TYPE_INTEGER) {
      json_field = strtol(field_data, NULL, 10);
      // Treat All Other Fields as Strings
    } else {
      json_field = field_data;
    }
  }
}

void process_setropts_field(char *field_data_destination,
                            char *field_data_source, int field_length) {
  memset(field_data_destination, 0, field_length + 1);
  copy_and_encode_string(field_data_destination, field_data_source,
                         field_length);
  trim_trailing_spaces(field_data_destination, field_length);
}

char get_setropts_field_type(char *field_key) {
  int list_length =
      sizeof(SETROPTS_FIELD_TYPES) / sizeof(SETROPTS_FIELD_TYPES[0]);
  for (int i = 0; i < list_length; i++) {
    if (strcmp(field_key, SETROPTS_FIELD_TYPES[i].key) == 0) {
      return SETROPTS_FIELD_TYPES[i].type;
    }
  }
  return SETROPTS_FIELD_TYPE_STRING;
}

std::string post_process_field_key(char *field_key, const char *profile_type,
                                   const char *segment,
                                   const char *raw_field_key) {
  post_process_key(field_key, raw_field_key, 8);
  const char *racfu_field_key = get_racfu_key(profile_type, segment, field_key);
  if (racfu_field_key == NULL) {
    return std::string("experimental:") + std::string(field_key);
  }
  return std::string(racfu_field_key);
}

void post_process_key(char *destination_key, const char *source_key,
                      int length) {
  copy_and_encode_string(destination_key, source_key, length);
  convert_to_lowercase(destination_key, length);
  trim_trailing_spaces(destination_key, length);
}

void copy_and_encode_string(char *destination_string, const char *source_string,
                            int length) {
  strncpy(destination_string, source_string, length);
#ifndef UNIT_TEST
  __e2a_l(destination_string, length);
#endif
}

void convert_to_lowercase(char *string, int length) {
  for (int i = 0; i < length; i++) {
    string[i] = tolower(string[i]);
  }
}

void trim_trailing_spaces(char *string, int length) {
  int i = length - 1;
  while (i >= 0) {
    if (string[i] == ' ') {
      string[i] = 0;
    } else {
      return;
    }
    i--;
  }
}
