#import cpyracf
import json
from ctypes import *
import os

class RACFAdmin:
    """Test class for Administration Interfaces"""

    def __init__(self, debug=False) -> None:
        self.__debug = debug
        file_path = f"{os.path.dirname(__file__)}/../../"
        cwd = os.getcwd()
        os.chdir(file_path)
        self.dll = CDLL(file_path+"/openRACFCore/corelib/lib/irrsmo00_conn.dll")
        os.chdir(cwd)
        
    
    def smo_from_dict(self, json_data: dict = {}, result_buffer_size: int = 1500, precheck: bool = False) -> str:

        if self.__debug:
            print(json.dumps(json_data))
        self.dll.call_irrsmo00_with_json.restype = c_char_p
        self.dll.call_irrsmo00_with_json.argtypes = [c_char_p, c_uint, c_uint, POINTER(c_uint), POINTER(c_uint), POINTER(c_uint), c_bool]

        json_req_string = c_char_p(json.dumps(json_data).encode("utf-8"))
        result_buffer_size = c_uint(result_buffer_size)
        irrsmo00_options = c_uint(15) if precheck else c_uint(13)
        saf_rc = c_uint(0)
        racf_rc = c_uint(0)
        racf_rsn = c_uint(0)
        debug = c_bool(1) if self.__debug else c_bool(0)
        json_res_string = self.dll.call_irrsmo00_with_json(json_req_string, result_buffer_size, irrsmo00_options, saf_rc, racf_rc, racf_rsn, debug)

        if self.__debug:
            print(f"SAF RC: {saf_rc.value} | RACF RC: {racf_rc.value} | RACF RSN: {racf_rsn.value}")
            print(json_res_string.decode("utf-8"))

        return {"resultBuffer": json_res_string.decode("utf-8"), "returnCodes": [saf_rc.value, racf_rc.value, racf_rsn.value]}
    
        #return cpyracf.call_irrsmo00_py(str(json.dumps()))