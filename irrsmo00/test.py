"""A sample file for testing the library's base functions."""
from cpyracf.test.test_admin import TestAdmin

test_admin = TestAdmin(debug=True)

print(test_admin.smo_from_file("cpyracf/utils/test_elijtest.json"))
print(test_admin.smo_from_file("cpyracf/utils/test_xfacilit.json"))
print(test_admin.smo_from_file("cpyracf/utils/test_facility.json"))
