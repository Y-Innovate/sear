import json
from racfu._C import call_racfu

class SecurityResult:
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

    
def racfu(request_dict: dict) -> dict:
    response = call_racfu(json.dumps(request_dict))
    return SecurityResult(
        request = request_dict,
        raw_request = response["raw_request"],
        raw_result = response["raw_result"],
        result = json.loads(response['result_json'])
    )
