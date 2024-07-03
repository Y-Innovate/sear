from pyRACFu_core import call_RACFu
import json

class RacfAdmin:
    def __init__(self, debug = False) -> None:
        self.__debug = debug
    
    def call_racf(self, request_dict: dict) -> dict:
        """Calls RACFu"""
        if self.__debug:
            request_dict["debug_mode"] = True

        response = call_RACFu(request_dict)
        response['result_json'] = json.loads(response['result_json'])
    
        if self.__debug:
            print(response)

        return response