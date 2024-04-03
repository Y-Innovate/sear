#include "../h/irrsmo00_conn.h"

void null_byte_fix(char* str, unsigned int str_len) {
   for (int i = 1; i < str_len; i++){
      if (str[i] == 0) {
         if (str[i-1] == 0x6E) {
            break; 
         }
         else {
            str[i] = 0x40;
         }
      }
   }
}

char * call_irrsmo00(
    char * request_xml, char * running_userid, unsigned int result_buffer_size, unsigned int irrsmo00_options,
    unsigned int * saf_rc, unsigned int * racf_rc, unsigned int * racf_rsn, bool debug
    )
{
    char work_area[1024];
    unsigned char req_handle[64] = {0};
    running_userid_t running_userid_struct = {strlen(running_userid), {0}};
    unsigned int alet = 0;
    unsigned int acee = 0;
    char  * result_buffer = malloc(result_buffer_size);
    memset(result_buffer, 0, result_buffer_size);
    unsigned int request_xml_length = strlen(request_xml);
    unsigned int result_len = result_buffer_size;
    unsigned int num_parms = 17;
    unsigned int fn = 1;

    strncpy(
        running_userid_struct.running_userid, 
        running_userid, 
        running_userid_struct.running_userid_length);
    
    if (debug && (running_userid_struct.running_userid_length > 0))
    {
        printf("Running under a userid!\n");
    }

    IRRSMO64(
        work_area,
        alet,
        saf_rc,
        alet,
        racf_rc,
        alet,
        racf_rsn,
        num_parms,
        fn,
        irrsmo00_options,
        request_xml_length,
        request_xml,
        req_handle,
        running_userid_struct,
        acee,
        &result_len,
        result_buffer);
    
    null_byte_fix(result_buffer, result_len);

    if ((*saf_rc != 8) || (*racf_rc != 4000) || ((*saf_rc == 8) && (*racf_rc == 4000) && (*racf_rsn > 100000000)))
    {
        return result_buffer;
    }

    unsigned int new_result_buffer_size = *racf_rsn + result_len + 1;
    if (debug) { printf("Reallocating Buffer of Size: %d\n", new_result_buffer_size); }
    char * full_result = malloc(new_result_buffer_size);
    char * result_buffer_ptr;
    memset(full_result, 0, new_result_buffer_size);
    strncpy(full_result, result_buffer, result_len);
    free(result_buffer);
    result_buffer_ptr = full_result + result_len * sizeof(unsigned char);
    result_len = *racf_rsn;

    // Call IRRSMO64 Again with the appropriate buffer size
    IRRSMO64(
        work_area,
        alet,
        saf_rc,
        alet,
        racf_rc,
        alet,
        racf_rsn,
        num_parms,
        fn,
        irrsmo00_options,
        request_xml_length,
        request_xml,
        req_handle,
        running_userid_struct,
        acee,
        &result_len,
        result_buffer_ptr);

    null_byte_fix(result_buffer_ptr, result_len);
    return full_result;
}

char * call_irrsmo00_with_json(
    char * json_req_string, unsigned int result_buffer_size, unsigned int irrsmo00_options,
    unsigned int * saf_rc, unsigned int * racf_rc, unsigned int * racf_rsn, bool debug
)
{
    char * running_userid = malloc(9);
    memset(running_userid, 0, 8);
    char * xml_res_string, *xml_req_string, * json_res_string;
    xml_req_string = injson_to_inxml(json_req_string, running_userid, debug);
    xml_res_string = call_irrsmo00(
        xml_req_string,
        running_userid,
        result_buffer_size,
        irrsmo00_options,
        saf_rc,
        racf_rc,
        racf_rsn,
        debug
    );
    free(running_userid);
    json_res_string = outxml_to_outjson(xml_res_string, debug);
    free(xml_res_string);
    return json_res_string;
}