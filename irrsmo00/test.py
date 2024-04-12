"""A sample file for testing the library's base functions."""
import json
import openRACFCore as core

def call_smo_from_file(file_name: str, debug: bool = False, buffer_size: int = 10000):
    test_admin = core.RACFAdmin(debug, buffer_size)
    file = open(file_name)
    data = json.load(file)

    return test_admin.smo_from_dict(data)


print(call_smo_from_file("utils/test_elijtest.json",True))
print(call_smo_from_file("utils/test_xfacilit.json"))
print(call_smo_from_file("utils/test_facility.json"))
