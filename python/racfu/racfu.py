import json
import racfu_py

class racfuResult:
    def __init__(
            self,
            request: dict,
            raw_request: bytes | None,
            raw_result: bytes | None,
            result: dict | None
    ):
        self.request = request
        self.raw_request = raw_request
        self.raw_result = raw_result
        self.result = result       

    
def racfu(request_dict: dict, debug_mode: bool = False) -> dict:
    """Calls racfu"""
    if debug_mode:
        request_dict["debug_mode"] = True

    response = racfu_py.call_racfu(json.dumps(request_dict))
    
    result = racfuResult(
        request = request_dict,
        raw_request = response["raw_request"],
        raw_result = response["raw_result"],
        result = json.loads(response['result_json'])
    )

    return result