#ifndef __RACFU_EXTRACT_H_
#define __RACFU_EXTRACT_H_

#include <cstdint>
#include <string>

#include "irrseq00.hpp"
#include "logger.hpp"
#include "messages.h"
#include "racfu_result.h"
#include "security_request.hpp"

void extract(RACFu::SecurityRequest &request, const Logger &logger);

generic_extract_underbar_arg_area_t *build_generic_extract_parms(
    const std::string &profile_name, const std::string &class_name,
    uint8_t function_code);

setropts_extract_underbar_arg_area_t *build_setropts_extract_parms();

void preserve_raw_request(const char *arg_area, char **raw_request,
                          const int &raw_request_length);

#endif
