
from helper import successful_return_codes, user_not_found_return_codes

# Import SEAR
from sear import sear


def test_add_user(delete_user):
    """This test is supposed to succeed"""
    add_result = sear(
            {
            "operation": "add", 
            "admin_type": "user", 
            "userid": delete_user,
            "traits": {
                "base:installation_data": "USER GENERATED DURING SEAR TESTING, NOT IMPORTANT",  # noqa: E501
            },
            },
        )
    assert "errors" not in str(add_result.result)
    assert add_result.result["return_codes"] == successful_return_codes

def test_extract_user(create_user):
    """This test is supposed to succeed"""
    extract_result = sear(
            {
            "operation": "extract",
            "admin_type": "user",
            "userid": create_user,
            },
        )
    assert "errors" not in str(extract_result.result)
    assert extract_result.result["return_codes"] == successful_return_codes

def test_user_extract_not_found():
    """This test is supposed to fail"""
    user_not_found_result = sear(
            {
            "operation": "extract",
            "admin_type": "user",
            "userid": "JMCCLANE",
            },
        )
    assert "errors" in str(user_not_found_result.result)
    assert user_not_found_result.result["return_codes"] == user_not_found_return_codes

def test_user_extract_missing_userid():
    """This test is supposed to fail"""
    user_not_found_result = sear(
            {
            "operation": "extract",
            "admin_type": "user",
            },
        )
    assert "errors" in str(user_not_found_result.result)
    assert user_not_found_result.result["return_codes"] != successful_return_codes

def test_alter_user(create_user):
    """This test is supposed to succeed"""
    alter_result = sear(
            {
            "operation": "alter", 
            "admin_type": "user", 
            "userid": create_user,
            "traits": {
                "omvs:default_shell": "/bin/zsh",
            },
            },
        )
    assert "errors" not in str(alter_result.result)
    assert alter_result.result["return_codes"] == successful_return_codes

def test_delete_user(create_user):
    """This test is supposed to succeed"""
    delete_result = sear(
            {
            "operation": "delete",
            "admin_type": "user",
            "userid": create_user,
            },
        )
    assert "errors" not in str(delete_result.result)
    assert delete_result.result["return_codes"] == successful_return_codes
