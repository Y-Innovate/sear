#ifndef __KEY_MAP_USER_H_
#define __KEY_MAP_USER_H_

#include "key_map_structs.hpp"

#include <stdbool.h>

const trait_key_mapping_t USER_BASE_SEGMENT_MAP[] {
  { 
    "base:automatic_data_set_protection",
    "adsp",
    TRAIT_TYPE_BOOLEAN },
  { 
    "base:auditor",
    "auditor",
    TRAIT_TYPE_BOOLEAN },
  { 
    "base:default_group_authority",
    "auth",
    TRAIT_TYPE_STRING },
  { 
    "base:security_category",
    "category",
    TRAIT_TYPE_STRING },
  { 
    "base:security_categories",
    "numctgy",
    TRAIT_TYPE_REPEAT },
  { 
    "base:class_authorization",
    "clauth",
    TRAIT_TYPE_STRING },
  { 
    "base:class_authorizations",
    "clcnt",
    TRAIT_TYPE_REPEAT },
  { 
    "base:group_connections",
    "connects",
    TRAIT_TYPE_REPEAT },
  { 
    "base:group_connection_automatic_data_set_protection",
    "cadsp",
    TRAIT_TYPE_BOOLEAN },
  { 
    "base:group_connection_auditor",
    "cauditor",
    TRAIT_TYPE_BOOLEAN },
  { 
    "base:group_connection_connect_date",
    "cauthda",
    TRAIT_TYPE_STRING },
  { 
    "base:group_connection_group",
    "cgroup",
    TRAIT_TYPE_STRING },
  { 
    "base:group_connection_data_set_access",
    "cgrpacc",
    TRAIT_TYPE_STRING },
  { 
    "base:group_connection_connects",
    "cinitct",
    TRAIT_TYPE_INTEGER},
  { 
    "base:group_connection_last_connect_date",
    "cljdate",
    TRAIT_TYPE_STRING },
  { "base:group_connection_last_connect_time",
    "cljtime",
    TRAIT_TYPE_STRING },
  { 
    "base:group_connection_operations",
    "coper",
    TRAIT_TYPE_BOOLEAN },
  { 
    "base:group_connection_owner",
    "cowner",
    TRAIT_TYPE_STRING },
  { 
    "base:group_connection_resume_date",
    "cresume",
    TRAIT_TYPE_STRING },
  { 
    "base:group_connection_revoke_date",
    "crevoke",
    TRAIT_TYPE_STRING },
  { 
    "base:group_connection_revoked",
    "crevokfl",
    TRAIT_TYPE_BOOLEAN },
  { 
    "base:group_connection_special",
    "cspecial",
    TRAIT_TYPE_BOOLEAN },
  { 
    "base:group_connection_universal_access",
    "cuacc",
    TRAIT_TYPE_STRING },
  { 
    "base:create_date",
    "creatdat",
    TRAIT_TYPE_STRING },
  { 
    "base:installation_data",
    "data",
    TRAIT_TYPE_STRING },
  { 
    "base:default_group",
    "dfltgrp",
    TRAIT_TYPE_STRING },
  { 
    "base:password_expired",
    "expired",
    TRAIT_TYPE_BOOLEAN },
  { 
    "base:mfa",
    "factorn",
    TRAIT_TYPE_REPEAT },
  { 
    "base:mfa_factor",
    "factor",
    TRAIT_TYPE_STRING },
  { 
    "base:mfa_active",
    "facactv",
    TRAIT_TYPE_BOOLEAN },
  { 
    "base:factor_tag_*",
    "factag*",
    TRAIT_TYPE_STRING },
  { 
    "base:factor_value_*",
    "facval*",
    TRAIT_TYPE_STRING },
  { 
    "base:group",
    "group",
    TRAIT_TYPE_STRING },
  { 
    "base:group_data_set_access",
    "grpacc",
    TRAIT_TYPE_BOOLEAN },
  { 
    "base:has_passphrase",
    "hasphras",
    TRAIT_TYPE_BOOLEAN},
  { 
    "base:has_password",
    "haspwd",
    TRAIT_TYPE_BOOLEAN },
  { 
    "base:last_access_date",
    "lastdate",
    TRAIT_TYPE_STRING },
  { 
    "base:last_acess_time",
    "lasttime",
    TRAIT_TYPE_STRING },
  { 
    "base:mfa_password_fallback",
    "mfaflbk",
    TRAIT_TYPE_BOOLEAN },
  { 
    "base:mfa_policy",
    "mfapolnm",
    TRAIT_TYPE_STRING },
  { 
    "base:mfa_policies",
    "mfapoln",
    TRAIT_TYPE_REPEAT },
  { 
    "base:model_data_set",
    "model",
    TRAIT_TYPE_STRING },
  { 
    "base:name",
    "name",
    TRAIT_TYPE_STRING },
  { 
    "base:require_operator_id_card",
    "oidcard",
    TRAIT_TYPE_BOOLEAN },
  { 
    "base:operations",
    "oper",
    TRAIT_TYPE_BOOLEAN },
  {
    "base:owner", 
    "owner",
    TRAIT_TYPE_STRING },
  {
    "base:password_change_date", 
    "passdate",
    TRAIT_TYPE_STRING },
  {
    "base:password_change_interval",
    "passint",
    TRAIT_TYPE_INTEGER },
  {
    "base:password", 
    "password",
    TRAIT_TYPE_STRING },
  {
    "base:passphrase", 
    "phrase",
    TRAIT_TYPE_STRING },
  {
    "base:passphrase_change_date",
    "phrdate",
    TRAIT_TYPE_STRING },
  {
    "base:passphrase_change_interval", 
    "phrint",
    TRAIT_TYPE_INTEGER },
  {
    "base:passphrase_enveloped", 
    "pphenv",
    TRAIT_TYPE_BOOLEAN },
  {
    "base:protected", 
    "protectd",
    TRAIT_TYPE_BOOLEAN },
  {
    "base:password_enveloped", 
    "pwdenv",
    TRAIT_TYPE_BOOLEAN },
  {
    "base:restrict_global_access_checking", 
    "rest",
    TRAIT_TYPE_BOOLEAN },
  {
    "base:resume_date", 
    "resume",
    TRAIT_TYPE_STRING },
  {
    "base:revoke_date", 
    "revoke",
    TRAIT_TYPE_STRING },
  {
    "base:revoked", 
    "revokefl",
    TRAIT_TYPE_BOOLEAN },
  {
    "base:audit_responsibility", 
    "roaudit",
    TRAIT_TYPE_BOOLEAN },
  {
    "base:security_label", 
    "seclabel",
    TRAIT_TYPE_STRING },
  {
    "base:security_level", 
    "seclevel",
    TRAIT_TYPE_STRING },
  {
    "base:special", 
    "special",
    TRAIT_TYPE_BOOLEAN },
  {
    "base:universal_access", 
    "uacc",
    TRAIT_TYPE_STRING },
  {
    "base:audit_logging", 
    "uaudit",
    TRAIT_TYPE_BOOLEAN },
  {
    "base:logon_allowed_day", 
    "whendays",
    TRAIT_TYPE_STRING },
  {
    "base:logon_allowed_days", 
    "whendyct",
    TRAIT_TYPE_REPEAT },
  {
    "base:logon_allowed_when_service", 
    "whensrv",
    TRAIT_TYPE_STRING },
  {
    "base:logon_allowed_time", 
    "whentime",
    TRAIT_TYPE_STRING }
};

const trait_key_mapping_t USER_OMVS_KEY_MAP[] = {
  {
    "omvs:max_address_space_size", 
    "assize", 
    TRAIT_TYPE_INTEGER },
  {
    "omvs:auto_uid", 
    "autouid", 
    TRAIT_TYPE_BOOLEAN },
  {
    "omvs:max_cpu_time", 
    "cputime", 
    TRAIT_TYPE_INTEGER },
  {
    "omvs:max_files_per_process", 
    "fileproc", 
    TRAIT_TYPE_INTEGER },
  {
    "omvs:home_directory", 
    "home", 
    TRAIT_TYPE_STRING },
  {
    "omvs:max_non_shared_memory", 
    "memlimit", 
    TRAIT_TYPE_STRING },
  {
    "omvs:max_file_mapping_pages", 
    "mmaparea", 
    TRAIT_TYPE_INTEGER },
  {
    "omvs:max_processes",
    "procuser", 
    TRAIT_TYPE_INTEGER },
  {
    "omvs:default_shell",
    "program",
    TRAIT_TYPE_STRING },
  {
    "omvs:shared",
    "shared",
    TRAIT_TYPE_BOOLEAN },
  {
    "omvs:max_shared_memory",
    "shmemmax",
    TRAIT_TYPE_STRING },
  {
    "omvs:max_threads", 
    "threads", 
    TRAIT_TYPE_INTEGER },
  {
    "omvs:uid", 
    "uid", 
    TRAIT_TYPE_INTEGER }
};

const trait_key_mapping_t USER_TSO_KEY_MAP[] = {
  {
    "tso:account_number", 
    "acctnum", 
    TRAIT_TYPE_STRING },
  {
    "tso:logon_command", 
    "command", 
    TRAIT_TYPE_STRING },
  {
    "tso:sysout_destination_id", 
    "dest", 
    TRAIT_TYPE_STRING },
  {
    "tso:hold_class", 
    "hldclass", 
    TRAIT_TYPE_STRING },
  {
    "tso:job_class", 
    "jobclass", 
    TRAIT_TYPE_STRING },
  {
    "tso:max_region_size", 
    "maxsize", 
    TRAIT_TYPE_INTEGER },
  {
    "tso:message_class", 
    "msgclass", 
    TRAIT_TYPE_STRING },
  {
    "tso:logon_procedure", 
    "proc", 
    TRAIT_TYPE_STRING },
  {
    "tso:security_label", 
    "seclabel", 
    TRAIT_TYPE_STRING },
  {
    "tso:default_region_size", 
    "size", 
    TRAIT_TYPE_INTEGER },
  {
    "tso:sysout_class", 
    "sysoutcl", 
    TRAIT_TYPE_STRING },
  {
    "tso:data_set_allocation_unit", 
    "unit", 
    TRAIT_TYPE_STRING },
  {
    "tso:user_data", 
    "userdata", 
    TRAIT_TYPE_STRING }
};

const segment_key_mapping_t USER_SEGMENT_KEY_MAP[] = {
  {
    "base", 
    field_count(USER_BASE_SEGMENT_MAP), 
    USER_BASE_SEGMENT_MAP },
  {
    "omvs", 
    field_count(USER_OMVS_KEY_MAP), 
    USER_OMVS_KEY_MAP },
  {
    "tso", 
    field_count(USER_TSO_KEY_MAP), 
    USER_TSO_KEY_MAP }
};

#endif
