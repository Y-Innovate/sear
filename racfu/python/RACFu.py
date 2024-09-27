from RACFu_py import call_RACFu
import json

class RACFAdmin:
    def __init__(self, debug = False, buffer_size = 10000) -> None:
        self.__debug = debug
        self.__buffer_size = buffer_size
    
    def call_racf(self, request_dict: dict) -> dict:
        """Calls RACFu"""
        if self.__debug:
            request_dict["debug_mode"] = True
        
        if not (self.__buffer_size == 10000):
            request_dict["result_buffer_size"] = self.__buffer_size

        response = call_RACFu(json.dumps(request_dict))
        response['result_json'] = json.loads(response['result_json'])
    
        if self.__debug:
            print(response)

        return response['result_json']