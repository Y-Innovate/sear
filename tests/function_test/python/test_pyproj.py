"""A sample file for testing the library's base functions."""
import json
from pprint import pprint
import os
from racfu import racfu

test_user = "SQUIDWRD"

irrseq00_request_failure = {
  "operation": "extract",
  "admin_type": "user",
  "profile_name": "SQUIDWRD"
}

irrseq00_request_success = {
  "operation": "extract",
  "admin_type": "user",
  "profile_name": f"{test_user}"
}

irrsmo00_request_failure = {
  "operation": "delete",
  "admin_type": "user",
  "profile_name": "SQUIDWRD"
}

irrsmo00_request_success = {
  "operation": "alter",
  "admin_type": "user",
  "profile_name": f"{test_user}",
  "traits": {
    "base:name": "Squilliam"
  }
}

add_user_for_testing = {
  "operation": "add",
  "admin_type": "user",
  "profile_name": "SQUIDWRD",
  "traits": {
    "base:name": "Squidward",
    "omvs:uid": 24,
    "omvs:home_directory": "/u/squidwrd"
  }
}

print("IRRSEQ00 FAILURE")
pprint(racfu(irrseq00_request_failure).result)
print("IRRSMO00 FAILURE")
pprint(racfu(irrsmo00_request_failure).result)

#Temporary measure to allow function testing successes in current environment
racfu(add_user_for_testing)

print("IRRSEQ00 SUCCESS")
pprint(racfu(irrseq00_request_success).result)
print("IRRSMO00 SUCCESS")
pprint(racfu(irrsmo00_request_success).result)

#Temporary measure to allow function testing successes in current environment
racfu(irrsmo00_request_failure)