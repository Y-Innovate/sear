"""A sample file for testing the library's base functions."""
import json
from pprint import pprint
import os
from RACFu import RACFAdmin

def call_smo_from_file(file_name: str, debug: bool = False, buffer_size: int = 10000):
    test_admin = RACFAdmin(debug)
    file = open(file_name)
    data = json.load(file)

    return test_admin.call_racf(data)

extract_path = "tests/irrseq00/request_samples/"
others_path = "tests/irrsmo00/request_samples/"

pprint(call_smo_from_file(extract_path+"test_extract_user_request.json"))
pprint(call_smo_from_file(others_path+"test_add_user_request.json"))
pprint(call_smo_from_file(extract_path+"test_extract_user_request.json"))
pprint(call_smo_from_file(others_path+"test_alter_user_request.json"))
pprint(call_smo_from_file(extract_path+"test_extract_user_request.json"))
pprint(call_smo_from_file(others_path+"test_delete_user_request.json"))
pprint(call_smo_from_file(extract_path+"test_extract_user_request.json"))