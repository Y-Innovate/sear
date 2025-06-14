
from helper import successful_return_codes

# Import SEAR
from sear import sear


def test_create_connect(create_user):
    """This test is supposed to succeed"""
    connect_result = sear(
            {
            "operation": "alter", 
            "admin_type": "group-connection", 
            "userid": create_user,
            "group": "SEARDUMY",
            "traits": {
                "base:owner": "SYS1",
            },
            },
        )
    assert "errors" not in str(connect_result.result)
    assert connect_result.result["return_codes"] == successful_return_codes

def test_create_connect_missing_group(create_user):
    """This test is supposed to fail"""
    connect_result = sear(
            {
            "operation": "alter", 
            "admin_type": "group-connection", 
            "userid": create_user,
            "traits": {
                "base:owner": "SYS1",
            },
            },
        )
    assert "errors" in str(connect_result.result)
    assert connect_result.result["return_codes"] != successful_return_codes