"""A sample file for testing the library's base functions."""
import json
import openracfcore as core

def call_smo_from_file(file_name: str, debug: bool = True):
    test_admin = core.RACFAdmin(debug=debug)
    file = open(file_name)
    data = json.load(file)

    return test_admin.smo_from_dict(data)


print(call_smo_from_file("utils/test_elijtest.json", debug = False))
print(call_smo_from_file("utils/test_xfacilit.json", debug = False))
print(call_smo_from_file("utils/test_facility.json", debug = False))
