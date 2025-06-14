
from helper import successful_return_codes

# Import SEAR
from sear import sear


def test_setropts_extract():
    """This test is supposed to succeed"""
    extract_result = sear(
        {
        "operation": "extract",
        "admin_type": "racf-options",
        },
    )
    assert "errors" not in str(extract_result.result)
    assert extract_result.result["return_codes"] == successful_return_codes
