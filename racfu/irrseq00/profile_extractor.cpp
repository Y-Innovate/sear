#include "profile_extractor.hpp"

#include <stdio.h>
#include <stdlib.h>

#include <cstring>

#include "racfu_error.hpp"

#ifdef __TOS_390__
#include <unistd.h>
#else
#include "zoslib.h"
#endif

// Use htonl() to convert 32-bit values from little endian to big endian.
// use ntohl() to convert 16-bit values from big endian to little endian.
// On z/OS these macros do nothing since "network order" and z/Architecture are
// both big endian. This is only necessary for unit testing off platform.
#define _POSIX_C_SOURCE 200112L
#include <arpa/inet.h>

namespace RACFu {
void ProfileExtractor::extract(SecurityRequest &request, const Logger &logger) {
  uint32_t rc;

  /*************************************************************************/
  /* RACF Options Extract                                                  */
  /*************************************************************************/
  if (request.function_code_ == SETROPTS_EXTRACT_FUNCTION_CODE) {
    // Build 31-bit Arg Area
    auto unique_ptr = make_unique31<setropts_extract_underbar_arg_area_t>();
    setropts_extract_underbar_arg_area_t *p_arg_area = unique_ptr.get();
    build_racf_options_extract_request(p_arg_area);
    // Preserve the raw request data
    request.p_result_->raw_request_length =
        (int)sizeof(setropts_extract_underbar_arg_area_t);
    logger.debug(MSG_REQUEST_SEQ_SETROPTS,
                 logger.cast_hex_string(reinterpret_cast<char *>(p_arg_area),
                                        request.p_result_->raw_request_length));

    preserve_raw_request(reinterpret_cast<char *>(p_arg_area),
                         &request.p_result_->raw_request,
                         request.p_result_->raw_request_length);

    logger.debug(MSG_CALLING_SEQ);

    // Call R_Admin
    rc = callRadmin(reinterpret_cast<char *__ptr32>(&p_arg_area->arg_pointers));
    logger.debug(MSG_DONE);

    request.p_result_->raw_result = p_arg_area->args.p_result_buffer;
    // Preserve Return & Reason Codes
    request.return_codes_.saf_return_code  = ntohl(p_arg_area->args.SAF_rc);
    request.return_codes_.racf_return_code = ntohl(p_arg_area->args.RACF_rc);
    request.return_codes_.racf_reason_code = ntohl(p_arg_area->args.RACF_rsn);
  }
  /***************************************************************************/
  /* Generic Extract                                                         */
  /*                                                                         */
  /* Use For:                                                                */
  /*   - User Extract                                                        */
  /*   - Group Extract                                                       */
  /*   - Group Connection Extract                                            */
  /*   - Resource Extract                                                    */
  /*   - Data Set Extract                                                    */
  /***************************************************************************/
  else {
    // Build 31-bit Arg Area
    auto unique_ptr = make_unique31<generic_extract_underbar_arg_area_t>();
    generic_extract_underbar_arg_area_t *p_arg_area = unique_ptr.get();
    build_generic_extract_request(p_arg_area, request.profile_name_,
                                  request.class_name_, request.function_code_);
    // Preserve the raw request data
    request.p_result_->raw_request_length =
        (int)sizeof(generic_extract_underbar_arg_area_t);
    logger.debug(MSG_REQUEST_SEQ_GENERIC,
                 logger.cast_hex_string(reinterpret_cast<char *>(p_arg_area),
                                        request.p_result_->raw_request_length));

    preserve_raw_request(reinterpret_cast<char *>(p_arg_area),
                         &request.p_result_->raw_request,
                         request.p_result_->raw_request_length);
    logger.debug(MSG_CALLING_SEQ);

    // Call R_Admin
    rc = callRadmin(reinterpret_cast<char *__ptr32>(&p_arg_area->arg_pointers));
    logger.debug(MSG_DONE);

    request.p_result_->raw_result = p_arg_area->args.p_result_buffer;
    // Preserve Return & Reason Codes
    request.return_codes_.saf_return_code  = ntohl(p_arg_area->args.SAF_rc);
    request.return_codes_.racf_return_code = ntohl(p_arg_area->args.RACF_rc);
    request.return_codes_.racf_reason_code = ntohl(p_arg_area->args.RACF_rsn);
  }

  // Check Return Codes
  if (request.return_codes_.saf_return_code != 0 ||
      request.return_codes_.racf_return_code != 0 ||
      request.return_codes_.racf_reason_code != 0 || rc != 0 ||
      request.p_result_->raw_result == nullptr) {
    request.return_codes_.racfu_return_code = 4;
    // Raise Exception if Extract Failed.
    if (request.admin_type_ != "racf-options") {
      throw RACFu::RACFuError("unable to extract '" + request.admin_type_ +
                              "' profile '" + request.profile_name_ + "'");
    } else {
      throw RACFu::RACFuError("unable to extract '" + request.admin_type_ +
                              "'");
    }
  }
  request.return_codes_.racfu_return_code = 0;
}

void ProfileExtractor::build_generic_extract_request(
    generic_extract_underbar_arg_area_t *arg_area,
    const std::string &profile_name, const std::string &class_name,
    uint8_t function_code) {
  // Make sure buffer is clear.
  memset(arg_area, 0, sizeof(generic_extract_underbar_arg_area_t));

  generic_extract_args_t *args                 = &arg_area->args;
  generic_extract_arg_pointers_t *arg_pointers = &arg_area->arg_pointers;
  generic_extract_parms_results_t *profile_extract_parms =
      &args->profile_extract_parms;

  /***************************************************************************/
  /* Set Extract Arguments                                                   */
  /***************************************************************************/
  args->ALET_SAF_rc           = ALET;
  args->ALET_RACF_rc          = ALET;
  args->ALET_RACF_rsn         = ALET;
  args->ACEE                  = ACEE;
  args->result_buffer_subpool = RESULT_BUFFER_SUBPOOL;
  args->function_code         = function_code;

  // Copy profile name and class name.
  memcpy(args->profile_name, profile_name.c_str(), profile_name.length());
  // Encode profile name as IBM-1047.
  __a2e_l(args->profile_name, profile_name.length());
  if (function_code == RESOURCE_EXTRACT_FUNCTION_CODE) {
    // Class name must be padded with blanks.
    memset(&profile_extract_parms->class_name, ' ', 8);
    memcpy(profile_extract_parms->class_name, class_name.c_str(),
           class_name.length());
    // Encode class name as IBM-1047.
    __a2e_l(profile_extract_parms->class_name, class_name.length());
  }
  profile_extract_parms->profile_name_length = htonl(profile_name.length());

  /***************************************************************************/
  /* Set Extract Argument Pointers                                           */
  /*                                                                         */
  /* Enable transition from 64-bit XPLINK to 31-bit OSLINK.                  */
  /***************************************************************************/
  arg_pointers->p_work_area =
      reinterpret_cast<char *__ptr32>(&args->RACF_work_area);
  arg_pointers->p_ALET_SAF_rc   = &(args->ALET_SAF_rc);
  arg_pointers->p_SAF_rc        = &(args->SAF_rc);
  arg_pointers->p_ALET_RACF_rc  = &(args->ALET_RACF_rc);
  arg_pointers->p_RACF_rc       = &(args->RACF_rc);
  arg_pointers->p_ALET_RACF_rsn = &(args->ALET_RACF_rsn);
  arg_pointers->p_RACF_rsn      = &(args->RACF_rsn);

  arg_pointers->p_function_code = &(args->function_code);
  // Function specific parms between function code and profile name
  arg_pointers->p_profile_name          = &(args->profile_name[0]);
  arg_pointers->p_ACEE                  = &(args->ACEE);
  arg_pointers->p_result_buffer_subpool = &(args->result_buffer_subpool);
  arg_pointers->p_p_result_buffer       = &(args->p_result_buffer);

  // Turn on the hight order bit of the last argument,
  // which marks the end of the argument list.
  *(reinterpret_cast<uint32_t *__ptr32>(&arg_pointers->p_p_result_buffer)) |=
      0x80000000;
  arg_pointers->p_profile_extract_parms = profile_extract_parms;
}

void ProfileExtractor::build_racf_options_extract_request(
    setropts_extract_underbar_arg_area_t *arg_area) {
  // Make sure buffer is clear.
  memset(arg_area, 0, sizeof(setropts_extract_underbar_arg_area_t));

  setropts_extract_args_t *args                 = &arg_area->args;
  setropts_extract_arg_pointers_t *arg_pointers = &arg_area->arg_pointers;
  setropts_extract_parms_t *setropts_extract_parms =
      &args->setropts_extract_parms;

  /***************************************************************************/
  /* Set Extract Arguments                                                   */
  /***************************************************************************/
  args->ALET_SAF_rc           = ALET;
  args->ALET_RACF_rc          = ALET;
  args->ALET_RACF_rsn         = ALET;
  args->ACEE                  = ACEE;
  args->result_buffer_subpool = RESULT_BUFFER_SUBPOOL;
  args->function_code         = SETROPTS_EXTRACT_FUNCTION_CODE;

  /***************************************************************************/
  /* Set Extract Argument Pointers                                           */
  /*                                                                         */
  /* Enable transition from 64-bit XPLINK to 31-bit OSLINK.                  */
  /***************************************************************************/
  arg_pointers->p_work_area =
      reinterpret_cast<char *__ptr32>(&args->RACF_work_area);
  arg_pointers->p_ALET_SAF_rc   = &(args->ALET_SAF_rc);
  arg_pointers->p_SAF_rc        = &(args->SAF_rc);
  arg_pointers->p_ALET_RACF_rc  = &(args->ALET_RACF_rc);
  arg_pointers->p_RACF_rc       = &(args->RACF_rc);
  arg_pointers->p_ALET_RACF_rsn = &(args->ALET_RACF_rsn);
  arg_pointers->p_RACF_rsn      = &(args->RACF_rsn);

  arg_pointers->p_function_code = &(args->function_code);
  // Function specific parms between function code and profile name
  arg_pointers->p_profile_name          = &(args->profile_name[0]);
  arg_pointers->p_ACEE                  = &(args->ACEE);
  arg_pointers->p_result_buffer_subpool = &(args->result_buffer_subpool);
  arg_pointers->p_p_result_buffer       = &(args->p_result_buffer);

  // Turn on the hight order bit of the last argument,
  // which marks the end of the argument list.
  *(reinterpret_cast<uint32_t *__ptr32>(&arg_pointers->p_p_result_buffer)) |=
      0x80000000;
  arg_pointers->p_setropts_extract_parms = setropts_extract_parms;
}

void ProfileExtractor::preserve_raw_request(const char *arg_area,
                                            char **raw_request,
                                            const int &raw_request_length) {
  *raw_request = static_cast<char *>(calloc(raw_request_length, sizeof(char)));
  if (*raw_request == NULL) {
    perror("Warn - Unable to allocate space to preserve the raw request.\n");
    return;
  }
  memcpy(*raw_request, arg_area, raw_request_length);
}
}  // namespace RACFu
