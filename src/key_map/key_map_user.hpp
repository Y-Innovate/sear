#ifndef __KEY_MAP_USER_H_
#define __KEY_MAP_USER_H_

#include "key_map_structs.hpp"

#include <stdbool.h>

const trait_key_mapping_t USER_OMVS_KEY_MAP[] = {
  { "omvs:max_address_space_size", "assize" },
  { "omvs:auto_uid", "autouid" },
  { "omvs:max_cpu_time", "cputime" },
  { "omvs:max_files_per_process", "fileproc" },
  { "omvs:home_directory", "home" },
  { "omvs:max_non_shared_memory", "memlimit" },
  { "omvs:max_file_mapping_pages", "mmaparea" },
  { "omvs:max_processes", "procuser" },
  { "omvs:default_shell", "program" },
  { "omvs:shared", "shared" },
  { "omvs:max_shared_memory", "shmemmax" },
  { "omvs:max_threads", "threads" },
  { "omvs:uid", "uid" },
};

const trait_key_mapping_t USER_TSO_KEY_MAP[] = {
  { "tso:account_number", "acctnum" },
  { "tso:logon_command", "command" },
  { "tso:sysout_destination_id", "dest" },
  { "tso:hold_class", "hldclass" },
  { "tso:job_class", "jobclass" },
  { "tso:max_region_size", "maxsize" },
  { "tso:message_class", "msgclass" },
  { "tso:logon_procedure", "proc" },
  { "tso:security_label", "seclabel" },
  { "tso:default_region_size", "size" },
  { "tso:sysout_class", "sysoutcl" },
  { "tso:data_set_allocation_unit", "unit" },
  { "tso:user_data", "userdata" },
};

const segment_key_mapping_t USER_SEGMENT_KEY_MAP[] = {
  { "omvs", field_count(USER_OMVS_KEY_MAP), USER_OMVS_KEY_MAP },
  { "tso", field_count(USER_TSO_KEY_MAP), USER_TSO_KEY_MAP }
};

#endif
