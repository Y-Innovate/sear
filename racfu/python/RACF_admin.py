#import cpyracf
import json
from ctypes import *
import os
import platform

class racf_result_t(Structure):
    _fields_ = [('raw_request', c_char_p), ('raw_request_length', c_int), ('raw_result', c_char_p), ('raw_result_length', c_int), ('result_json',c_char_p)]

class RACFAdmin:
    """Test class for Administration Interfaces"""

    def __init__(self, debug=False, buffer_size=10000) -> None:
        self.__debug = debug
        self.__buffer_size = buffer_size
        file_path = f"{os.path.dirname(__file__)}/../../dist/"
        cwd = os.getcwd()
        os.chdir(file_path)
        self.dll = CDLL(file_path+"racfu.so")
        os.chdir(cwd)
        
    
    def smo_from_dict(self, json_data: dict = {}) -> str:

        if self.__debug:
            print(json.dumps(json_data))
            json_data["debug_mode"] = True
        
        if not (platform.system() == "OS/390"):
            self.__buffer_size = 1500
        if not (self.__buffer_size == 10000):
            json_data["result_buffer_size"] = self.__buffer_size

        self.dll.racfu.argtypes = [POINTER(racf_result_t),
                                                     c_char_p]

        json_req_string = c_char_p(json.dumps(json_data).encode("utf-8"))
        results = racf_result_t(c_char_p("".encode("utf-8")),0,c_char_p("".encode("utf-8")))
        self.dll.racfu(results, json_req_string)

        json_res_string = results.result_json

        if self.__debug:
            print(json_res_string.decode("utf-8"))

        return json.loads(json_res_string.decode("utf-8"))
    
        #return cpyracf.call_irrsmo00_py(str(json.dumps()))
