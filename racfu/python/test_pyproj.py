"""A sample file for testing the library's base functions."""
import json
import os
from pyRACFu import RACFAdmin

def call_smo_from_file(file_name: str, debug: bool = False, buffer_size: int = 10000):
    test_admin = RACFAdmin(debug)
    file = open(file_name)
    data = json.load(file)

    print(data)
    return test_admin.call_racf(data)

print(call_smo_from_file("tests/irrseq00/request_samples/test_extract_user_request.json"))
print(call_smo_from_file("tests/irrsmo00/request_samples/test_add_user_request.json"))
print(call_smo_from_file("tests/irrseq00/request_samples/test_extract_user_request.json"))
print(call_smo_from_file("tests/irrsmo00/request_samples/test_alter_user_request.json"))
print(call_smo_from_file("tests/irrseq00/request_samples/test_extract_user_request.json"))
print(call_smo_from_file("tests/irrsmo00/request_samples/test_delete_user_request.json"))
print(call_smo_from_file("tests/irrseq00/request_samples/test_extract_user_request.json"))