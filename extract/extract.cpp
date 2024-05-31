#include "extract.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *extract(
    char *profile_name,   // Required for everything except setropts
    char *class_name,     // Only required for general resource profile
    uint8_t function_code // Always required
) {
  int rc;

  char *result_buffer;
  int result_buffer_length;

  /*************************************************************************/
  /* Setropts Extract                                                      */
  /*************************************************************************/
  if (function_code == SETROPTS_EXTRACT_FUNCTION_CODE) {
    // Build 31-bit Arg Area
    setropts_extract_underbar_arg_area_t *arg_area_setropts;
    arg_area_setropts = build_setropts_extract_parms();
    if (arg_area_setropts == NULL) { return NULL; }
    // Call R_Admin
    rc = callRadmin((char ZOS_PTR_32) & arg_area_setropts->arg_pointers);
    setropts_extract_results_t *setropts_result_buffer =
        (setropts_extract_results_t *) arg_area_setropts->args.pResult_buffer;
    result_buffer = (char *) setropts_result_buffer;
    // Check Return and Reason Codes
    rc = check_return_and_reason_codes(
        (char *) arg_area_setropts,
        result_buffer,
        rc,
        arg_area_setropts->args.SAF_rc,
        arg_area_setropts->args.RACF_rc,
        arg_area_setropts->args.RACF_rsn);
    free(arg_area_setropts);
    if (rc != 0) { return NULL; }
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
  } else {
    // Build 31-bit Arg Area
    generic_extract_underbar_arg_area_t *arg_area_generic;
    arg_area_generic = build_generic_extract_parms(
        profile_name,
        class_name,
        function_code);
    if (arg_area_generic == NULL) { return NULL; }
    // Call R_Admin
    rc = callRadmin((char ZOS_PTR_32) & arg_area_generic->arg_pointers);
    generic_extract_parms_results_t *generic_result_buffer =
        (generic_extract_parms_results_t *)
            arg_area_generic->args.pResult_buffer;
    result_buffer = (char *)generic_result_buffer;
    // Check Return and Reason Code
    rc = check_return_and_reason_codes(
        (char *) arg_area_generic,
        result_buffer,
        rc,
        arg_area_generic->args.SAF_rc,
        arg_area_generic->args.RACF_rc,
        arg_area_generic->args.RACF_rsn);
    free(arg_area_generic);
    if (rc != 0) { return NULL; }
  }

  // Return Result if Successful
  return result_buffer;
}

int check_return_and_reason_codes(
    char *arg_area,
    char *result_buffer,
    uint32_t rc,
    uint32_t SAF_rc,
    uint32_t RACF_rc,
    uint32_t RACF_rsn)
{
  if (
      rc != 0 
      || SAF_rc != 0 
      || RACF_rc != 0 
      || RACF_rsn != 0
  ) {
    printf(
        "Fatal - IRRSEQ00 (R_Admin) service was unsuccessful.\n"
        "RC = %d\n"
        "SAF RC = %d\n"
        "RACF RC = %d\n"
        "RACF RSN = %d\n",
        rc,
        SAF_rc,
        RACF_rc,
        RACF_rsn);
    free(result_buffer);
    free(arg_area);
    return -1;
  }
  return 0;
}

generic_extract_underbar_arg_area_t *build_generic_extract_parms(
    char *profile_name,    // Required always.
    char *class_name,      // Required only for resource extract.
    uint8_t function_code  // Required always.
) {
  int profile_name_length;
  if (profile_name != NULL) {
    profile_name_length = strlen(profile_name);
  }
  int class_name_length;
  if (class_name != NULL) {
    class_name_length = strlen(class_name);
  }

  /***************************************************************************/
  /* ISO8859-1 to IBM-1047 Encoding Conversions                              */
  /***************************************************************************/
  #ifndef UNIT_TEST
  if (ascii_to_ebcdic(profile_name) != 0) {
    return NULL;
  }
  if (ascii_to_ebcdic(class_name) != 0) {
    return NULL;
  }
  #endif

  /***************************************************************************/
  /* Allocate 31-bit Area For IRRSEQ00 Parameters/Arguments                  */
  /***************************************************************************/
  generic_extract_underbar_arg_area_t *arg_area;
  arg_area = (generic_extract_underbar_arg_area_t *)
      ZOS_MALLOC_31(sizeof(generic_extract_underbar_arg_area_t));
  if (arg_area == NULL) {
    perror(
        "Fatal - Unable to allocate space in 31-bit storage "
        "for 'generic_extract_underbar_arg_area_t'.\n");
    return NULL;
  }

  generic_extract_args_t *args = &arg_area->args;
  generic_extract_arg_pointers_t *arg_pointers = &arg_area->arg_pointers;
  generic_extract_parms_results_t *profile_extract_parms =
      &args->profile_extract_parms;

  /***************************************************************************/
  /* Set Extract Arguments                                                   */
  /***************************************************************************/
  SET_COMMON_ARGS
  args->function_code = function_code;

  // Copy profile name and class name.
  memcpy(
      args->profile_name,
      profile_name,
      profile_name_length);
  if (class_name != NULL)
  {
    // Class name must be padded with blanks.
    memset(&profile_extract_parms->class_name, ' ', 8);
    memcpy(
        profile_extract_parms->class_name,
        class_name,
        class_name_length);
  }
  profile_extract_parms->profile_name_length = profile_name_length;

  /***************************************************************************/
  /* Set Extract Argument Pointers                                           */
  /*                                                                         */
  /* Enable transition from 64-bit XPLINK to 31-bit OSLINK.                  */
  /***************************************************************************/
  SET_COMMON_ARG_POINTERS
  arg_pointers->pProfile_extract_parms = profile_extract_parms;

  return arg_area;
}

setropts_extract_underbar_arg_area_t *build_setropts_extract_parms() {
  /***************************************************************************/
  /* Allocate 31-bit Area For IRRSEQ00 Parameters/Arguments                  */
  /***************************************************************************/
  setropts_extract_underbar_arg_area_t *arg_area;
  arg_area = (setropts_extract_underbar_arg_area_t *)
      ZOS_MALLOC_31(sizeof(setropts_extract_underbar_arg_area_t));
  if (arg_area == NULL) {
    perror(
        "Fatal - Unable to allocate space in 31-bit storage "
        "for 'setropts_extract_underbar_arg_area_t'.\n");
    return NULL;
  }

  setropts_extract_args_t *args = &arg_area->args;
  setropts_extract_arg_pointers_t *arg_pointers = &arg_area->arg_pointers;
  setropts_extract_parms_t *setropts_extract_parms = &args->setropts_extract_parms;

  /***************************************************************************/
  /* Set Extract Arguments                                                   */
  /***************************************************************************/
  SET_COMMON_ARGS
  args->function_code = SETROPTS_EXTRACT_FUNCTION_CODE;

  /***************************************************************************/
  /* Set Extract Argument Pointers                                           */
  /*                                                                         */
  /* Enable transition from 64-bit XPLINK to 31-bit OSLINK.                  */
  /***************************************************************************/
  SET_COMMON_ARG_POINTERS
  arg_pointers->pSetropts_extract_parms = setropts_extract_parms;

  return arg_area;
}

#ifndef UNIT_TEST
int ascii_to_ebcdic(char *string)
{
  // Skip encoding conversion if 'string' is 'NULL'.
  if (string == NULL) { return 0; }
  int rc = __a2e_s(string);
  if (rc == -1) {
    perror("");
    printf(
        "Fatal - Unable to convert '%s' from "
        "ISO8859-1 to IBM-1047 encoding.\n",
        string);
    return -1;
  }
  return 0;
}
#endif
