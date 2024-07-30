#include "extract.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *extract(
    const char *profile_name,  // Required for everything except setropts
    const char *class_name,    // Only required for general resource profile
    uint8_t function_code,     // Always required
    racfu_return_codes_t *return_codes  // Always required
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
        if (arg_area_setropts == NULL) {
            return NULL;
        }
        // Call R_Admin
        rc = callRadmin((char ZOS_PTR_32) & arg_area_setropts->arg_pointers);
        setropts_extract_results_t *setropts_result_buffer =
            (setropts_extract_results_t *)
                arg_area_setropts->args.pResult_buffer;
        result_buffer = (char *)setropts_result_buffer;
        // Preserve Return & Reason Codes
        return_codes->saf_return_code = arg_area_setropts->args.SAF_rc;
        return_codes->racf_return_code = arg_area_setropts->args.RACF_rc;
        return_codes->racf_reason_code = arg_area_setropts->args.RACF_rsn;
        return_codes->irrseq00_return_code = rc;
        // Free Arg Area
        free(arg_area_setropts);
        /***************************************************************************/
        /* Generic Extract */
        /*                                                                         */
        /* Use For: */
        /*   - User Extract */
        /*   - Group Extract */
        /*   - Group Connection Extract */
        /*   - Resource Extract */
        /*   - Data Set Extract */
        /***************************************************************************/
    } else {
        // Build 31-bit Arg Area
        generic_extract_underbar_arg_area_t *arg_area_generic;
        arg_area_generic = build_generic_extract_parms(profile_name, class_name,
                                                       function_code);
        if (arg_area_generic == NULL) {
            return NULL;
        }
        // Call R_Admin
        rc = callRadmin((char ZOS_PTR_32) & arg_area_generic->arg_pointers);
        generic_extract_parms_results_t *generic_result_buffer =
            (generic_extract_parms_results_t *)
                arg_area_generic->args.pResult_buffer;
        result_buffer = (char *)generic_result_buffer;
        // Preserve Return & Reason Codes
        return_codes->saf_return_code = arg_area_generic->args.SAF_rc;
        return_codes->racf_return_code = arg_area_generic->args.RACF_rc;
        return_codes->racf_reason_code = arg_area_generic->args.RACF_rsn;
        return_codes->irrseq00_return_code = rc;
        // Free Arg Area
        free(arg_area_generic);
    }

    // Check Return Codes
    if (return_codes->saf_return_code != 0 ||
        return_codes->racf_return_code != 0 ||
        return_codes->racf_reason_code != 0 ||
        return_codes->irrseq00_return_code != 0) {
        // Free Result Buffer & Return 'NULL' if not successful.
        free(result_buffer);
        return NULL;
    }

    // Return Result if Successful
    return result_buffer;
}

generic_extract_underbar_arg_area_t *build_generic_extract_parms(
    const char *profile_name,  // Required always.
    const char *class_name,    // Required only for resource extract.
    uint8_t function_code      // Required always.
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
    /* Allocate 31-bit Area For IRRSEQ00 Parameters/Arguments */
    /***************************************************************************/
    generic_extract_underbar_arg_area_t *arg_area;
    arg_area = (generic_extract_underbar_arg_area_t *)ZOS_MALLOC_31(
        sizeof(generic_extract_underbar_arg_area_t));
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
    /* Set Extract Arguments */
    /***************************************************************************/
    SET_COMMON_ARGS
    args->function_code = function_code;

    // Copy profile name and class name.
    memcpy(args->profile_name, profile_name, profile_name_length);
// Encode profile name as IBM-1047.
#ifndef UNIT_TEST
    __a2e_l(args->profile_name, profile_name_length);
#endif
    if (class_name != NULL) {
        // Class name must be padded with blanks.
        memset(&profile_extract_parms->class_name, ' ', 8);
        memcpy(profile_extract_parms->class_name, class_name,
               class_name_length);
// Encode class name as IBM-1047.
#ifndef UNIT_TEST
        __a2e_l(profile_extract_parms->class_name, class_name_length);
#endif
    }
    profile_extract_parms->profile_name_length = profile_name_length;

    /***************************************************************************/
    /* Set Extract Argument Pointers */
    /*                                                                         */
    /* Enable transition from 64-bit XPLINK to 31-bit OSLINK. */
    /***************************************************************************/
    SET_COMMON_ARG_POINTERS
    arg_pointers->pProfile_extract_parms = profile_extract_parms;

    return arg_area;
}

setropts_extract_underbar_arg_area_t *build_setropts_extract_parms() {
    /***************************************************************************/
    /* Allocate 31-bit Area For IRRSEQ00 Parameters/Arguments */
    /***************************************************************************/
    setropts_extract_underbar_arg_area_t *arg_area;
    arg_area = (setropts_extract_underbar_arg_area_t *)ZOS_MALLOC_31(
        sizeof(setropts_extract_underbar_arg_area_t));
    if (arg_area == NULL) {
        perror(
            "Fatal - Unable to allocate space in 31-bit storage "
            "for 'setropts_extract_underbar_arg_area_t'.\n");
        return NULL;
    }

    setropts_extract_args_t *args = &arg_area->args;
    setropts_extract_arg_pointers_t *arg_pointers = &arg_area->arg_pointers;
    setropts_extract_parms_t *setropts_extract_parms =
        &args->setropts_extract_parms;

    /***************************************************************************/
    /* Set Extract Arguments */
    /***************************************************************************/
    SET_COMMON_ARGS
    args->function_code = SETROPTS_EXTRACT_FUNCTION_CODE;

    /***************************************************************************/
    /* Set Extract Argument Pointers */
    /*                                                                         */
    /* Enable transition from 64-bit XPLINK to 31-bit OSLINK. */
    /***************************************************************************/
    SET_COMMON_ARG_POINTERS
    arg_pointers->pSetropts_extract_parms = setropts_extract_parms;

    return arg_area;
}
