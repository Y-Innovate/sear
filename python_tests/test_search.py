
from helper import successful_return_codes

# Import SEAR
from sear import sear


def test_search_resource_profiles_class_missing():
    """This test is supposed to fail"""
    add_result = sear(
            {
            "operation": "search", 
            "admin_type": "resource", 
            },
        )
    assert "errors" in str(add_result.result)
    assert add_result.result["return_codes"] != successful_return_codes