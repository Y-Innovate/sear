#ifndef __RACFU_UNIT_TEST_UTILITIES_
#define __RACFU_UNIT_TEST_UTILITIES_

#include <iostream>

char *get_sample(const char *filename, const char *mode);
char *get_raw_sample(const char *filename);
std::string get_json_sample(const char *filename);

// IRRSEQ00
void check_arg_pointers(char *raw_request, bool racf_options);
void test_extract_request_generation(const char *test_extract_request_json,
                                     const char *test_extract_request_raw,
                                     bool racf_options);
void test_parse_extract_request(const char *test_extract_request_json,
                                const char *test_extract_result_json,
                                const char *test_extract_result_raw);
void test_parse_extract_result_profile_not_found(
    const char *test_extract_request_json,
    const char *test_extract_result_profile_not_found_json);

#endif
