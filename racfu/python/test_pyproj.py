"""A sample file for testing the library's base functions."""
import json
import os
from pyRACFu import RACFAdmin

def call_smo_from_file(file_name: str, debug: bool = False, buffer_size: int = 10000):
    test_admin = RACFAdmin(debug)
    file = open(file_name)
    data = json.load(file)

    return test_admin.smo_from_dict(data)

print(call_smo_from_file("tests/sample_json/test_extract_user.json"))
print(call_smo_from_file("tests/sample_json/test_add_user.json"))
print(call_smo_from_file("tests/sample_json/test_extract_user.json"))
print(call_smo_from_file("tests/sample_json/test_alter_user.json"))
print(call_smo_from_file("tests/sample_json/test_extract_user.json"))
print(call_smo_from_file("tests/sample_json/test_delete_user.json"))
print(call_smo_from_file("tests/sample_json/test_extract_user.json"))