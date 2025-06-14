
from helper import keyring_not_found_return_codes, successful_return_codes

# Import SEAR
from sear import sear


def test_extract_keyring_not_found():
    """This test is supposed to fail"""
    not_found_result = sear(
        {
        "operation": "extract", 
        "admin_type": "keyring", 
        "keyring": "SEARNOTFOUND",
        "owner": "IBMUSER",
        },
    )
    assert "errors" in str(not_found_result.result)
    assert not_found_result.result["return_codes"] == keyring_not_found_return_codes

def test_extract_keyring_missing_admin_type():
    """This test is supposed to fail"""
    not_found_result = sear(
        {
        "operation": "extract", 
        "keyring": "SEARNOTFOUND",
        "owner": "IBMUSER",
        },
    )
    assert "errors" in str(not_found_result.result)
    assert not_found_result.result["return_codes"] != keyring_not_found_return_codes

def test_extract_keyring_missing_operation():
    """This test is supposed to fail"""
    not_found_result = sear(
        {
        "admin_type": "keyring", 
        "keyring": "SEARNOTFOUND",
        "owner": "IBMUSER",
        },
    )
    assert "errors" in str(not_found_result.result)
    assert not_found_result.result["return_codes"] != keyring_not_found_return_codes

def test_extract_keyring(create_keyring):
    """This test is supposed to succeed"""
    keyring, owner = create_keyring
    extract_result = sear(
        {
        "operation": "extract", 
        "admin_type": "keyring", 
        "keyring": keyring,
        "owner": owner,
        },
    )
    assert "errors" not in str(extract_result.result)
    assert extract_result.result["return_codes"] == successful_return_codes

def test_add_keyring(delete_keyring):
    """This test is supposed to succeed"""
    keyring, owner = delete_keyring
    add_result = sear(
        {
        "operation": "add", 
        "admin_type": "keyring", 
        "keyring": keyring,
        "owner": owner,
        },
    )
    assert "errors" not in str(add_result.result)
    assert add_result.result["return_codes"] == successful_return_codes

def test_delete_keyring(create_keyring):
    """This test is supposed to succeed"""
    keyring, owner = create_keyring
    delete_result = sear(
        {
        "operation": "delete", 
        "admin_type": "keyring", 
        "keyring": keyring,
        "owner": owner,
        },
    )
    assert "errors" not in str(delete_result.result)
    assert delete_result.result["return_codes"] == successful_return_codes
