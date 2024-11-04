from racfu import racfu
import json
import os

# This user shouldn't exist
if "RACFU_FVT_USERID" in os.environ:
  test_user = os.environ["RACU_FVT_USERID"]
else:
  print(
    "The 'RACFU_FVT_USERID' environment variable must be set "
    + "to a z/OS userid that does NOT exist on the system.")
  exit(1)

# This request will fail, but it demonstrates that 
# we can make a request to IRRSEQ00 and get a result back.
extract_request = {
  "admin_type": "user",
  "operation": "extract",
  "profile_name": test_user
}

# This request will fail, but it demonstrates that
# we can make a request to IRRSMO00 and get a result back.
alter_request = {
  "admin_type": "user",
  "operation": "alter",
  "profile_name": test_user,
  "traits": {
    "base:name": "Squilliam"
  }
}

print("Extract Test (IRRSEQ00):")
result = racfu(extract_request)
print(json.dumps(result.result, indent=2))

print("Alter Test (IRRSMO00):")
result = racfu(alter_request)
print(json.dumps(result.result, indent=2))
