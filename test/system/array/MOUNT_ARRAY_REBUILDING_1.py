#!/usr/bin/env python3
import subprocess
import os
import sys
sys.path.append("../lib/")

import json_parser
import ibofos
import ibofos_util
import cli
import test_result
import json
import time
import CREATE_ARRAY_BASIC_1

def clear_result():
    if os.path.exists( __file__ + ".result"):
        os.remove( __file__ + ".result")

def check_result(detail):
    data = json.loads(detail)
    state = data['Response']['info']['situation']
    if state == "REBUILDING":
        return "pass"
    return "fail"

def set_result(detail):
    out = detail
    code = json_parser.get_response_code(detail)
    result = test_result.expect_true(code)
    if result == "pass":
        out = cli.get_ibofos_info()
        result = check_result(out)
    
    with open(__file__ + ".result", "w") as result_file:
        result_file.write(result + " (" + str(code) + ")" + "\n" + out)

def execute():
    clear_result()
    CREATE_ARRAY_BASIC_1.execute()
    ibofos_util.pci_detach(CREATE_ARRAY_BASIC_1.ANY_DATA)
    time.sleep(0.1)
    out = cli.mount_ibofos()
    return out

if __name__ == "__main__":
    out = execute()
    set_result(out)
    ibofos.kill_ibofos()
    ibofos_util.pci_rescan()