#include "irrsmo00.hpp"

#include <stdlib.h>
#include <string.h>

#include "xml_generator.hpp"
#include "xml_parser.hpp"

char *call_irrsmo00(char *request_xml, char *running_userid,
                    unsigned int *result_buffer_size, int irrsmo00_options,
                    int *saf_rc, int *racf_rc, int *racf_rsn, bool debug) {
  char work_area[1024];
  char req_handle[64] = {0};
  running_userid_t running_userid_struct = {
      (unsigned char)strlen(running_userid), {0}};
  unsigned int alet = 0;
  unsigned int acee = 0;
  char *result_buffer =
      static_cast<char *>(calloc(*result_buffer_size, sizeof(char)));
  int request_xml_length = strlen(request_xml);
  int result_len = *result_buffer_size;
  int num_parms = 17;
  int fn = 1;

  strncpy(running_userid_struct.running_userid, running_userid,
          running_userid_struct.running_userid_length);

  if (debug && (running_userid_struct.running_userid_length > 0)) {
    printf("Running under a userid!\n");
  }

  IRRSMO64(work_area, alet, saf_rc, alet, racf_rc, alet, racf_rsn, &num_parms,
           &fn, &irrsmo00_options, &request_xml_length, request_xml, req_handle,
           reinterpret_cast<char *>(&running_userid_struct), acee, &result_len,
           result_buffer);

  if (((*saf_rc != 8) || (*racf_rc != 4000)) ||
      ((*saf_rc == 8) && (*racf_rc == 4000) && (*racf_rsn > 100000000))) {
    *result_buffer_size = result_len;
    return result_buffer;
  }

  unsigned int new_result_buffer_size = *racf_rsn + result_len + 1;
  if (debug) {
    printf("Reallocating Buffer of Size: %d\n", new_result_buffer_size);
  }
  char *full_result =
      static_cast<char *>(calloc(new_result_buffer_size, sizeof(char)));
  char *result_buffer_ptr;
  strncpy(full_result, result_buffer, result_len);
  free(result_buffer);
  result_buffer_ptr = full_result + result_len * sizeof(unsigned char);
  *result_buffer_size = result_len;
  result_len = *racf_rsn;

  // Call IRRSMO64 Again with the appropriate buffer size
  IRRSMO64(work_area, alet, saf_rc, alet, racf_rc, alet, racf_rsn, &num_parms,
           &fn, &irrsmo00_options, &request_xml_length, request_xml, req_handle,
           reinterpret_cast<char *>(&running_userid_struct), acee, &result_len,
           result_buffer_ptr);

  *result_buffer_size += result_len;
  return full_result;
}