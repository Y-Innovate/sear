#ifndef __RACFU_UNIT_TEST_UTILITIES_
#define __RACFU_UNIT_TEST_UTILITIES_

#include <iostream>

char *get_sample(const char *filename, const char *mode);
char *get_raw_sample(const char *filename);
std::string get_json_sample(const char *filename);

#endif
