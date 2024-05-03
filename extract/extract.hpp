#ifndef __EXTRACT_H_
#define __EXTRACT_H_

#include <stdint.h>

#ifndef UNIT_TEST
    #include <unistd.h>

    #define ZOS_PTR_32      * __ptr32
    #define ZOS_MALLOC_31   __malloc31
#else
    #define ZOS_PTR_32      *
    #define ZOS_MALLOC_31   malloc
#endif


/*************************************************************************/
/* Function Codes                                                        */
/*************************************************************************/
const uint8_t SETROPTS_EXTRACT_FUNCTION_CODE            = 0x16;
const uint8_t USER_EXTRACT_FUNCTION_CODE                = 0x19;
const uint8_t GROUP_EXTRACT_FUNCTION_CODE               = 0x1b;
const uint8_t GROUP_CONNECTION_EXTRACT_FUNCTION_CODE    = 0x1d;
const uint8_t RESOURCE_EXTRACT_FUNCTION_CODE            = 0x1f;
const uint8_t DATA_SET_EXTRACT_FUNCTION_CODE            = 0x22;


/*************************************************************************/
/* Field Descriptor Information                                          */
/*************************************************************************/
// Field types
const uint16_t t_member_repeat_group    = 0x8000;   // member of a repeat group
const uint16_t t_reserved               = 0x4000;   // reserved
const uint16_t t_boolean_field          = 0x2000;   // flag (boolean) field
const uint16_t t_repeat_field_header    = 0x1000;   // repeat field header

// Field descriptor flags
const uint32_t f_boolean_field      = 0x80000000;   // value of a boolean field
const uint32_t f_output_only        = 0x40000000;   // output-only field


/*************************************************************************/
/* Common Aliases                                                        */
/*************************************************************************/
const uint8_t RESULT_BUFFER_SUBPOOL = 127;
const uint32_t ALET = 0x00000000;  // primary address space
const uint32_t ACEE = 0x00000000;

/*************************************************************************/
/* Common Structure Data Macros                                          */
/*************************************************************************/
#define PROFILE_NAME_MAX_LENGTH 247

#define COMMON_START_ARGS \
    char RACF_work_area[1024]; \
    /* return and reason codes */ \
    int  ALET_SAF_rc; \
    int  SAF_rc; \
    int  ALET_RACF_rc; \
    int  RACF_rc; \
    int  ALET_RACF_rsn; \
    int  RACF_rsn; \
    /* extract function to perform */ \
    unsigned char function_code;

#define COMMON_START_ARG_POINTERS \
    char ZOS_PTR_32 pWork_area; \
    /* return and reason code */ \
    int ZOS_PTR_32 pALET_SAF_rc; \
    int ZOS_PTR_32 pSAF_rc; \
    int ZOS_PTR_32 pALET_RACF_rc; \
    int ZOS_PTR_32 pRACF_rc; \
    int ZOS_PTR_32 pALET_RACF_rsn; \
    int ZOS_PTR_32 pRACF_rsn; \
    /* extract function to perform */ \
    unsigned char ZOS_PTR_32 pFunction_code;

#define COMMON_END_ARGS \
    /* Max of 247 + 1 for null terimnator */ \
    char profile_name[PROFILE_NAME_MAX_LENGTH + 1]; \
    /* Result area for the service */ \
    unsigned int ACEE; \
    unsigned char result_buffer_subpool; \
    /* R_admin returns data here */ \
    char ZOS_PTR_32 pResult_buffer;

#define COMMON_END_ARG_POINTERS \
    char ZOS_PTR_32 pProfile_name; \
    /* Result area for the service */ \
    unsigned int ZOS_PTR_32 pACEE; \
    unsigned char ZOS_PTR_32 pResult_buffer_subpool; \
    /* R_admin returns data here */ \
    char ZOS_PTR_32 ZOS_PTR_32 ppResult_buffer;

#define SET_COMMON_ARGS \
    args->ALET_SAF_rc           = ALET; \
    args->ALET_RACF_rc          = ALET; \
    args->ALET_RACF_rsn         = ALET; \
    args->ACEE                  = ACEE; \
    args->result_buffer_subpool = RESULT_BUFFER_SUBPOOL;

#define SET_COMMON_ARG_POINTERS \
    arg_pointers->pWork_area = (char ZOS_PTR_32)&args->RACF_work_area; \
    arg_pointers->pALET_SAF_rc = &(args->ALET_SAF_rc); \
    arg_pointers->pSAF_rc = &(args->SAF_rc); \
    arg_pointers->pALET_RACF_rc = &(args->ALET_RACF_rc); \
    arg_pointers->pRACF_rc = &(args->RACF_rc); \
    arg_pointers->pALET_RACF_rsn = &(args->ALET_RACF_rsn); \
    arg_pointers->pRACF_rsn = &(args->RACF_rsn); \
    \
    arg_pointers->pFunction_code = &(args->function_code); \
    /* Function specific parms between function code and profile name */ \
    arg_pointers->pProfile_name = &(args->profile_name[0]); \
    arg_pointers->pACEE = &(args->ACEE); \
    arg_pointers->pResult_buffer_subpool = &(args->result_buffer_subpool); \
    arg_pointers->ppResult_buffer = &(args->pResult_buffer); \
    \
    /* Turn on the hight order bit of the last argument - marks the end of the */ \
    /* argument list. */ \
    *((unsigned int ZOS_PTR_32)&arg_pointers->ppResult_buffer) |= 0x80000000;


#pragma pack(packed)    // Don't byte align structure members.

/*************************************************************************/
/* Generic Extract Structures                                            */
/*                                                                       */
/* Use For:                                                              */
/*   - User Extract                                                      */
/*   - Group Extract                                                     */
/*   - Group Connection Extract                                          */
/*   - Resource Extract                                                  */
/*   - Data Set Extract                                                  */
/*************************************************************************/
typedef struct {
    char            eyecatcher[4];          // 'PXTR'
    int             result_buffer_length;   // result buffer length
    unsigned char   subpool;                // subpool of result buffer
    unsigned char   version;                // parameter list version
    unsigned char   reserved_1[2];          // reserved
    char            class_name[8];          // class name - upper case, blank pad
    int             profile_name_length;    // length of profile name
    unsigned char   reserved_2[2];          // reserved
    char            volume[6];              // volume (for data set extract)
    unsigned char   reserved_3[4];          // reserved
    unsigned int    flags;                  // see flag constants below
    int             segment_count;          // number of segments
    unsigned char   reserved_4[16];         // reserved
    // start of extracted data
} generic_extract_parms_results_t;
// Note: This structure is used for both input & output.

typedef struct {
    COMMON_START_ARGS
    generic_extract_parms_results_t profile_extract_parms;
    COMMON_END_ARGS
} generic_extract_args_t;

typedef struct {
    COMMON_START_ARG_POINTERS
    generic_extract_parms_results_t ZOS_PTR_32 pProfile_extract_parms;
    COMMON_END_ARG_POINTERS
} generic_extract_arg_pointers_t;

// 31-bit for IRRSEQ00 arguments.
typedef struct {
   generic_extract_args_t args;
   generic_extract_arg_pointers_t arg_pointers;
} generic_extract_underbar_arg_area_t;


/*************************************************************************/
/* Setropts Extract Structures                                           */
/*                                                                       */
/* Specific to Setropts Extract.                                         */
/*************************************************************************/
typedef struct {
    unsigned int    request_flags;
    unsigned char   reserved_1[10];
} setropts_extract_parms_t;

typedef struct {
    char            eyecatcher[4];
    int             result_buffer_length;
    unsigned char   reserved_2[4];
    unsigned short  segment_count;
    // Start of extracted data.
} setropts_extract_results_t;

typedef struct {
    COMMON_START_ARGS
    setropts_extract_parms_t setropts_extract_parms;
    COMMON_END_ARGS
} setropts_extract_args_t;

typedef struct {
    COMMON_START_ARG_POINTERS
    setropts_extract_parms_t ZOS_PTR_32 pSetropts_extract_parms;
    COMMON_END_ARG_POINTERS
} setropts_extract_arg_pointers_t;

// 31-bit for IRRSEQ00 arguments.
typedef struct {
   setropts_extract_args_t args;
   setropts_extract_arg_pointers_t arg_pointers;
} setropts_extract_underbar_arg_area_t;

/*************************************************************************/
/* Segment/Field Descriptor Structures                                   */
/*                                                                       */
/* Used to interpret extracted generic profile data                      */
/*************************************************************************/
typedef struct {
    char     name[8];                   // segment name, upper case, blank padded
    uint32_t flags;                     //
    uint32_t field_count;               // number of fields
    char     reserved_1[4];             // reserved
    uint32_t field_descriptor_offset;   // offset to first field descriptor
    char     reserved_2[16];            // reserved
                                        // start of next segment descriptor
} segment_descriptor_t;

typedef union {
    uint32_t field_data_length;          // length of field data or ...
    uint32_t repeat_group_count;         // number of repeat groups
} field_data_length_repeat_group_count_t;

typedef union {
    uint32_t field_data_offset;          // offset to field data or ...
    uint32_t repeat_group_element_count; // number of elems in repeat field hdrs
} field_data_offset_repeat_group_element_count_t;

typedef struct {
    char     name[8];       // field name, upper case, blank padded
    uint16_t type;
    char     reserved_1[2];
    uint32_t flags;
    field_data_length_repeat_group_count_t 
        field_data_length_repeat_group_count;
    char     rserved_2[4];
    field_data_offset_repeat_group_element_count_t
        field_data_offset_repeat_group_element_count;
    char     reserved_3[16];
                            // start of next field descriptor
} field_descriptor_t;

#pragma pack(reset)     // Restore default structure packing options.


// Glue code to call IRRSEQ00 assembler code.
extern "C" int callRadmin(char ZOS_PTR_32);

char * extract(
    char *profile_name, 
    char *class_name, 
    unsigned char function_code);

int check_return_and_reason_codes(
    char *arg_area,
    char *result_buffer,
    int rc,
    int SAF_rc,
    int RACF_rc,
    int RACF_rsn);

generic_extract_underbar_arg_area_t *build_generic_extract_parms(
    char *profile_name,
    char *class_name,
    char function_code);

setropts_extract_underbar_arg_area_t *build_setropts_extract_parms();

#ifndef UNIT_TEST
int ascii_to_ebcdic(char *string);
#endif

#endif