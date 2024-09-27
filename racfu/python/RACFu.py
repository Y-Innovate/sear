import json
from typing import Union
import RACFu_py

class RACFuResult:
    def __init__(
            self,
            request: dict,
            raw_request: Union[bytes,None],
            raw_result: Union[bytes,None],
            result: Union[dict,None]
    ):
        self.request = request
        self.raw_request = raw_request
        self.raw_result = raw_result
        self.result = result       

    def __str__(self):
        return f"{self.request=}\n{self.raw_request=}\n{self.raw_result=}\n{self.result=}\n" 

class RACFu:
    def __init__(self, debug = False, buffer_size = 10000) -> None:
        self.__debug = debug
        self.__buffer_size = buffer_size
    
    def make_request(self, request_dict: dict) -> dict:
        """Calls RACFu"""
        if self.__debug:
            request_dict["debug_mode"] = True
        
        if not (self.__buffer_size == 10000):
            request_dict["result_buffer_size"] = self.__buffer_size

        response = RACFu_py.call_RACFu(json.dumps(request_dict))
    
        if self.__debug:
            print(response)
        
        result = RACFuResult(
            request = request_dict,
            raw_request = response["raw_request"],
            raw_result = response["raw_result"],
            result = json.loads(response['result_json'])
        )

        if self.__debug:
            print(result)

        return result