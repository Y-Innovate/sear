"""A sample file for testing the library's base functions."""
import json
import os
from RACF_admin import RACFAdmin

def call_smo_from_file(file_name: str, debug: bool = False, buffer_size: int = 10000):
    test_admin = RACFAdmin(debug, buffer_size)
    file = open(file_name)
    data = json.load(file)

    return test_admin.smo_from_dict(data)

print(call_smo_from_file("tests/sample_json/test_add_resource_surrogate.json"))
print(call_smo_from_file("tests/sample_json/test_add_resource.json"))
print(call_smo_from_file("tests/sample_json/test_delete_resource.json"))
