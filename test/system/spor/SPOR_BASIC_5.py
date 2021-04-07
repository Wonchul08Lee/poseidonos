# !/usr/bin/env python3

import sys
import os
import time
import subprocess

from itertools import product
from threading import Thread

import TEST
import TEST_FIO
import TEST_LIB
import TEST_LOG
import TEST_SETUP_IBOFOS

volumes = [1]
current_test = 0
run_time = 10

############################################################################
## Test Description
##  write pattern to the volume for (run_time) secs,
##  simulate SPOR before all writes are completed (WRITE FAIL EXPECTED),
##  and do read write to see pos works properly
############################################################################
def test(offset, size, testIdx):
    global current_test
    current_test = current_test + 1
    TEST_LOG.print_notice("[{} - Test {} Started]".format(filename, current_test))

    for volId in volumes:
        th = Thread(target=TEST_FIO.write, args=(volId, offset, size, testIdx, run_time))
        th.start()

    time.sleep(run_time / 2)
    TEST_SETUP_IBOFOS.trigger_spor()
    th.join()

    TEST_SETUP_IBOFOS.dirty_bringup()
    for volId in volumes:
        TEST_SETUP_IBOFOS.create_subsystem(volId)
        TEST_SETUP_IBOFOS.mount_volume(volId)

    for volId in volumes:
        TEST_FIO.write(volId, offset, size, testIdx + 1)
    for volId in volumes:
        TEST_FIO.verify(volId, offset, size, testIdx + 1)

    TEST_LOG.print_notice("[Test {} Completed]".format(current_test))

def execute():
    offsets = [0, 4096]
    sizes = ['128k', '256k']

    test(offset=0, size='1m', testIdx=1)
    if TEST.quick_mode == False:
        for i, (_offset, _size) in enumerate(product(offsets, sizes)):
            test(offset=_offset, size=_size, testIdx=i+1)

if __name__ == "__main__":
    global filename
    filename = sys.argv[0].split("/")[-1].split(".")[0]
    TEST_LIB.set_up(argv=sys.argv, test_name=filename)

    TEST_SETUP_IBOFOS.clean_bringup()
    for volId in volumes:
        TEST_SETUP_IBOFOS.create_subsystem(volId)
        TEST_SETUP_IBOFOS.create_volume(volId)

    execute()

    TEST_LIB.tear_down(test_name=filename)