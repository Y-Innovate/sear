#ifndef __POST_PROCESS_H_
#define __POST_PROCESS_H_

#include "extract.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

json post_process_generic(
    generic_extract_parms_results_t *generic_result_buffer);

json post_process_setropts(
    setropts_extract_results_t *setropts_result_buffer);

bool process_boolean_field(
    generic_field_descriptor_t *field,
    char *field_key);

void process_generic_field(
    generic_field_descriptor_t *field, 
    char *field_key,
    char *field_data,
    char *profile_address);

char get_setropts_field_type(char *field_key);

void post_process_key(
    char *destination_key, 
    const char *source_key, 
    int length);

void copy_and_encode_string(
    char *destination_string, 
    const char *source_string, 
    int length);

void convert_to_lowercase(char * string, int length);

void trim_trailing_spaces(char *string, int length);

#endif
