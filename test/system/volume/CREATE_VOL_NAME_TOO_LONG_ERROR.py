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
import MOUNT_ARRAY_BASIC_1

def clear_result():
    if os.path.exists( __file__ + ".result"):
        os.remove( __file__ + ".result")

def set_result(detail):
    code = json_parser.get_response_code(detail)
    result = test_result.expect_false(code)
    with open(__file__ + ".result", "w") as result_file:
        result_file.write(result + " (" + str(code) + ")" + "\n" + detail)

def execute():
    clear_result()
    MOUNT_ARRAY_BASIC_1.execute()
    size = ibofos_constant.SIZE_1GB
    long_name = "aaaaaaaabbbbbbbbccccccccddddddddaaaaaaaabbbbbbbbccccccccddddddddaaaaaaaabbbbbbbbccccccccddddddddaaaaaaaabbbbbbbbccccccccddddddddaaaaaaaabbbbbbbbccccccccddddddddaaaaaaaabbbbbbbbccccccccddddddddaaaaaaaabbbbbbbbccccccccddddddddaaaaaaaabbbbbbbbccccccccdddddddd"
    out = cli.create_volume(long_name, str(size),"","","")
    return out

if __name__ == "__main__":
    out = execute()
    set_result(out)
    ibofos.kill_ibofos()