#import cpyracf
import json
from ctypes import *
import os
import platform

class RACFAdmin:
    """Test class for Administration Interfaces"""

    def __init__(self, debug=False) -> None:
        self.__debug = debug
        file_path = f"{os.path.dirname(__file__)}/../.."
        cwd = os.getcwd()
        os.chdir(file_path)
        if platform.system() == "OS/390":
            self.dll = CDLL(file_path+"/openRACFCore/corelib/lib/irrsmo00_conn.so")
        else:
            self.dll = CDLL(file_path+"/openRACFCore/corelib/lib/irrsmo00_conn.dll")
        os.chdir(cwd)
        
    
    def smo_from_dict(self, json_data: dict = {}) -> str:

        if self.__debug:
            print(json.dumps(json_data))
            json_data[list(json_data)[0]]["debugmode"] = True

        self.dll.call_irrsmo00_with_json.restype = c_char_p
        self.dll.call_irrsmo00_with_json.argtypes = [c_char_p]

        json_req_string = c_char_p(json.dumps(json_data).encode("utf-8"))
        json_res_string = self.dll.call_irrsmo00_with_json(json_req_string)

        if self.__debug:
            print(json_res_string.decode("utf-8"))

        return json.loads(json_res_string.decode("utf-8"))
    
        #return cpyracf.call_irrsmo00_py(str(json.dumps()))