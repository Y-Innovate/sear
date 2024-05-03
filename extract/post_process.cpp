#include "post_process.hpp"

#include <iostream>

#include <ctype.h>
#include <stdio.h>
#include <string.h>

json post_process_generic(
    generic_extract_parms_results_t *generic_result_buffer
) {
    json profile;
    profile["profile"] = json::object();
    char *profile_address = (char *)generic_result_buffer;
    
    // Set Class Name
    char class_name[8];
    copy_and_encode_string(
        class_name, 
        generic_result_buffer->class_name, 
        8);
    trim_trailing_spaces(class_name, 8);
    profile["class"] = class_name;

    // Segment Variables
    int first_segment_offset = sizeof(generic_extract_parms_results_t);
    first_segment_offset += generic_result_buffer->profile_name_length;
    segment_descriptor_t *segment = 
        (segment_descriptor_t *) (profile_address + first_segment_offset);
    char segment_key[8];

    // Field Variables
    field_descriptor_t *field;
    char field_key[8];
    char field_data[1025];
    int field_length;

    // Repeat Group Variables
    std::vector<json> repeat_group;
    int repeat_group_count;
    int repeat_group_element_count;
    char repeat_field_key[8];

    // Post Process Segments
    for (int i = 1; i <= generic_result_buffer->segment_count; i++) {
        post_process_key(segment_key, segment->name, 8);
        profile["profile"][segment_key] = json::object();

        // Post Process Fields
        field = 
            (field_descriptor_t *)
            (profile_address + segment->field_descriptor_offset);
        for (int j = 1; j <= segment->field_count; j++) {
            post_process_key(field_key, field->name, 8);
            // Post Process Boolean Fields
            if (field->type & t_boolean_field) {
                profile["profile"][segment_key][field_key] = 
                    process_boolean_field(
                        field,
                        field_key);
            // Post Process Generic Fields
            } else if (!(field->type & t_repeat_field_header)) {
                process_generic_field(
                    field,
                    field_key,
                    field_data,
                    profile_address);
                profile["profile"][segment_key][field_key] = field_data;
            // Post Process Repeat Fields
            } else {
                repeat_group_count = field->
                    field_data_length_repeat_group_count.repeat_group_count;
                repeat_group_element_count = 
                    field->field_data_offset_repeat_group_element_count.repeat_group_element_count;
                field++;
                // Post Process Each Repeat Group
                for (int k = 1; k <= repeat_group_count; k++) {
                    repeat_group.push_back(json::object());
                    // Post Process Each Repeat Group Field
                    for (int l = 1; l <= repeat_group_element_count; l++) {
                        post_process_key(repeat_field_key, field->name, 8);
                        // Assume that repeat fields just get processed as generic field?
                        process_generic_field(
                            field,
                            repeat_field_key,
                            field_data,
                            profile_address);
                        repeat_group[k-1][repeat_field_key] = field_data;
                        field++;
                    }
                }
                profile["profile"][segment_key][field_key] = repeat_group;
                repeat_group.clear();
            }
            field++;
        }
        segment++;
    }
    return profile;
}

bool process_boolean_field(
        field_descriptor_t *field, 
        char *field_key
) {
    if (field->flags & f_boolean_field) {
        return true;
    } else {
        return false;
    }
}

void process_generic_field(
        field_descriptor_t *field, 
        char *field_key,
        char *field_data,
        char *profile_address
) {
    int field_length = field->
        field_data_length_repeat_group_count.field_data_length;
    memset(field_data, 0, field_length+1);
    copy_and_encode_string(
        field_data, 
        profile_address + field->
            field_data_offset_repeat_group_element_count.field_data_offset,
        field_length);
}

void post_process_key(
        char *destination_key, 
        const char *source_key, 
        int length
) {
    copy_and_encode_string(destination_key, source_key, length);
    convert_to_lowercase(destination_key, length);
    trim_trailing_spaces(destination_key, length);
}

void copy_and_encode_string(
        char *destination_string, 
        const char *source_string, 
        int length
) {
    strncpy(destination_string, source_string, length);
    #ifndef UNIT_TEST
    __e2a_l(destination_string, length);
    #endif
}

void convert_to_lowercase(char * string, int length) {
    for (int i = 0; i < length; i++) {
        string[i] = tolower(string[i]);
    }
}

void trim_trailing_spaces(char *string, int length) {
    int i = length-1;
    while (i >= 0) {
        if (string[i] == ' ') {
            string[i] = 0;
        } else {
            return;
        }
        i--;
    }
}