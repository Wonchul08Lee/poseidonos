#!/usr/bin/env python3
import subprocess
import os
import sys
import json
sys.path.append("../lib/")
sys.path.append("../array/")

import json_parser
import ibofos
import cli
import test_result
import ibofos_constant
import CREATE_VOL_BASIC_1

def clear_result():
    if os.path.exists( __file__ + ".result"):
        os.remove( __file__ + ".result")

def check_result(detail):
    data = json.loads(detail)
    description = data['Response']['result']['status']['description']
    if "no any" in description:
        return "pass"
    return "fail"

def set_result(detail):
    result = check_result(detail)
    code = json_parser.get_response_code(detail)
    with open(__file__ + ".result", "w") as result_file:
        result_file.write(result + " (" + str(code) + ")" + "\n" + detail)

def execute():
    clear_result()
    CREATE_VOL_BASIC_1.execute()
    cli.unmount_ibofos()
    out = cli.list_volume("")
    return out

if __name__ == "__main__":
    out = execute()
    set_result(out)
    ibofos.kill_ibofos()