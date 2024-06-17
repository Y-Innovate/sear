#ifndef __RACFU_H_
#define __RACFU_H_

extern "C" {
  void racfu(racf_result_t *result, char *request_json);
}

#pragma export(racfu)

#endif
