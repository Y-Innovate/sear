#include "irrsmo64.hpp"

extern void IRRSMO64(char work_area[1024], unsigned int alet_saf_rc,
                     int* saf_rc, unsigned int alet_racf_rc, int* racf_rc,
                     unsigned int alet_racf_rsn, int* racf_rsn, int* num_parms,
                     int* fn, int* irrsmo00_options, int* request_xml_length,
                     char* request_xml, char* request_handle, char* userid,
                     unsigned int acee, int* result_len, char* result_buffer) {
  *saf_rc = 0;
  *racf_rc = 0;
  *racf_rsn = 0;
  *result_len = 0;
  return;
}