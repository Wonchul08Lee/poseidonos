#!/usr/bin/env python3

import subprocess
from subprocess import check_call, call, check_output, Popen, PIPE
import random
import os
import sys
import re
import signal
import getopt
from datetime import datetime
from itertools import *
import csv
import itertools
from shutil import copyfile
import json
import optparse

#######################################################################################
# edit test parameters into these lists to run different workloads
ibof_root = os.path.dirname(os.path.abspath(__file__)) + "/../"
ioengine = ibof_root + "lib/spdk-19.10/examples/nvme/fio_plugin/fio_plugin"

# 1 namespace
traddr="172.16.1.1"
trtype="tcp"
#filename="trtype=tcp adrfam=IPv4 traddr=" + traddr + " trsvcid=1158 subnqn=nqn.2019-04.ibof\:subsystem1 ns=1:"
#filename='trtype=pcie traddr=0000.02.00.0 ns=1'

# the configuration below runs QD 1 & 128. 
# To add set q_depth=['1', '32', '128']
q_depth=['32']

# io_size and block_size specifies the size in bytes of the IO workload.
# To add 64K IOs set io_size = ['4096', '65536'] and block_size = [ '512', '1024', '4096', '512-128k' ]
io_size_bytes=['20m']

block_size=['4032']

# type of I/O pattern : write, read, trim, randread, randwrite, randtrim, readwrite, rw, randrw
readwrite=['write']

# verify = True | False. applied on sorts of write I/O patterns
verify=False

# run_time parameter specifies how long to run each test.
# Set run_time = ['10', '600'] to run the test for given seconds
run_time=['5']

# mixed rw ratio
mix=['100']

# cpu affinity to run fio (comma-delimited)
cpus_allowed=['']

# iter_num parameter is used to run the test multiple times.
# set iter_num = ['1', '2', '3'] to repeat each test 3 times
iter_num=['1']

# setting profile_mode True | False. True will remains profile json file and .csv result file
profile_mode=False

# verbose = True | False. setting True will show more fio log
verbose=True

numjobs='4'
time_based='1'
ramp_time='2'
file_num=1

# extra fio options
extra_fio_options="--thread=1 --group_reporting=1 --direct=1"

#######################################################################################

red = "\033[1;31m"
green = "\033[0;32m"
reset = "\033[0;0m"

def run_fio(filename, io_size_bytes, block_size, qd, rw_mix, cpus_allowed, run_num, workload, run_time_sec):
    sys.stdout.write(red)
    print("[TEST {}] ".format(run_num), end='')
    sys.stdout.write(green)
    print("Started. size={} block_size={} qd={} io_pattern={} mix={} cpus_allowed={} time={}".format(io_size_bytes, block_size, qd, workload, rw_mix, cpus_allowed, run_time_sec))
    sys.stdout.write(reset)
    print("")

    # call fio

    command = "fio "\
            + " --ioengine=" + str(ioengine) + "" \
            + " --runtime=" + str(run_time_sec) + "" \
            + " --io_size=" + str(io_size_bytes) + "" \
            + " --iodepth=" + str(qd) + "" \
            + " --readwrite=" + str(workload) + ""

    if cpus_allowed != "":
        command += " --cpus_allowed=" + str(cpus_allowed) + ""

    if block_size.find('-') == -1:
        command += " --bs_unaligned=1 --bs=" + str(block_size)
    else:
        command += " --bsrange=" + str(block_size)

    if workload.find('rw') == -1:
        command += " --norandommap=1 "

    if verify == True:
        command += " --verify=md5 "
        command += " --serialize_overlap=1 "
    else:
        command += " --verify=0 "

    command += " --time_based=" + str(time_based)+" ";
    command += " --ramp_time=" + str(ramp_time)+" ";
    command += " --numjobs=" + str(numjobs)+" ";
    command += " --size=1gb ";

    if profile_mode == True:
        string = "size_" + str(io_size_bytes) + "_bs_" + str(block_size) + "_qd_" + str(qd) + "_mix_" + str(rw_mix) + "_workload_" + str(workload) + "_run_" + str(run_num)
        command += " --output=" + string + " --output-format=json "

    command += extra_fio_options

    if file_num > 1:
        for i in range(0, file_num):
            command += " --name=test" + str(i) + " --filename='trtype=" + str(trtype) + " adrfam=IPv4 traddr=" + str(traddr) + " trsvcid=1158 subnqn=nqn.2019-04.ibof\:subsystem" + str(i+1) + " ns=1'"
    else:
        command += " --name=test --filename='" + filename + "'"

    if verbose == True:
        print(command)

    ret = subprocess.call(command, shell=True)
    sys.stdout.write(red)
    if ret != 0:
        print("Test {} Failed".format(run_num))
        sys.stdout.write(reset)
        sys.exit(1);
    else:
        print("[TEST {}] ".format(run_num), end='')
        sys.stdout.write(green)
        print("Success. size={} block_size={} qd={} io_pattern={} mix={} cpus_allowed={} runtime={}\n".format(io_size_bytes, block_size, qd, workload, rw_mix, cpus_allowed, run_time_sec))
        sys.stdout.write(reset)
    return


def parse_results(io_size_bytes, block_size, qd, rw_mix, cpus_allowed, run_num, workload, run_time_sec):
    results_array = []

    # if json file has results for multiple fio jobs pick the results from the right job
    job_pos = 0

    # generate the next result line that will be added to the output csv file
    results = str(io_size_bytes) + "," + str(block_size) + "," + str(qd) + "," + str(rw_mix) + "," \
            + str(workload) + "," + str(cpus_allowed) + "," + str(run_time_sec) + "," + str(run_num)

    # read the results of this run from the test result file
    string = "size_" + str(io_size_bytes) + "_bs_" + str(block_size) + "_qd_" + str(qd) + "_mix_" + str(rw_mix) + "_workload_" + str(workload) + "_run_" + str(run_num)
    with open(string) as json_file:
        data = json.load(json_file)
        job_name = data['jobs'][job_pos]['jobname']
        # print "fio job name: ", job_name
        if 'lat_ns' in data['jobs'][job_pos]['read']:
            lat = 'lat_ns'
            lat_units = 'ns'
        else:
            lat = 'lat'
            lat_units = 'us'
        read_iops = float(data['jobs'][job_pos]['read']['iops'])
        read_bw = float(data['jobs'][job_pos]['read']['bw'])
        read_avg_lat = float(data['jobs'][job_pos]['read'][lat]['mean'])
        read_min_lat = float(data['jobs'][job_pos]['read'][lat]['min'])
        read_max_lat = float(data['jobs'][job_pos]['read'][lat]['max'])
        write_iops = float(data['jobs'][job_pos]['write']['iops'])
        write_bw = float(data['jobs'][job_pos]['write']['bw'])
        write_avg_lat = float(data['jobs'][job_pos]['write'][lat]['mean'])
        write_min_lat = float(data['jobs'][job_pos]['write'][lat]['min'])
        write_max_lat = float(data['jobs'][job_pos]['write'][lat]['max'])
        print("%-10s" % "io size", "%-10s" % "block size", "%-10s" % "qd", "%-10s" % "mix",
                "%-10s" % "workload type", "%-10s" % "cpu mask",
                "%-10s" % "run time", "%-10s" % "run num",
                "%-15s" % "read iops",
                "%-10s" % "read mbps", "%-15s" % "read avg. lat(" + lat_units + ")",
                "%-15s" % "read min. lat(" + lat_units + ")", "%-15s" % "read max. lat(" + lat_units + ")",
                "%-15s" % "write iops",
                "%-10s" % "write mbps", "%-15s" % "write avg. lat(" + lat_units + ")",
                "%-15s" % "write min. lat(" + lat_units + ")", "%-15s" % "write max. lat(" + lat_units + ")")
        print("%-10s" % io_size_bytes, "%-10s" % block_size, "%-10s" % qd, "%-10s" % rw_mix,
                "%-10s" % workload, "%-10s" % cpus_allowed, "%-10s" % run_time_sec,
                "%-10s" % run_num, "%-15s" % read_iops, "%-10s" % read_bw,
                "%-15s" % read_avg_lat, "%-15s" % read_min_lat, "%-15s" % read_max_lat,
                "%-15s" % write_iops, "%-10s" % write_bw, "%-15s" % write_avg_lat,
                "%-15s" % write_min_lat, "%-15s" % write_max_lat)
        results = results + "," + str(read_iops) + "," + str(read_bw) + "," \
                + str(read_avg_lat) + "," + str(read_min_lat) + "," + str(read_max_lat) \
                + "," + str(write_iops) + "," + str(write_bw) + "," + str(write_avg_lat) \
                + "," + str(write_min_lat) + "," + str(write_max_lat)
        with open(result_file_name, "a") as result_file:
            result_file.write(results + "\n")
        results_array = []
    return


def get_nvme_devices_count():
    output = check_output('lspci | grep -i Non | wc -l', shell=True)
    return int(output)


def get_nvme_devices_bdf():
    output = check_output('lspci | grep -i Non | awk \'{print $1}\'', shell=True).decode("utf-8")
    output = output.split()
    return output


def add_filename_to_conf(conf_file_name, bdf):
    filestring = "filename=trtype=PCIe traddr=0000." + bdf.replace(":", ".") + " ns=1"
    with open(conf_file_name, "a") as conf_file:
        conf_file.write(filestring + "\n")


# parse option
parser = optparse.OptionParser()
parser.add_option("-t", "--transport", dest="transport", help="nvmf transport name", default="tcp")
parser.add_option("-i", "--ip", dest="ip", help="nvmf listen ip", default="10.100.11.1")
parser.add_option("-p", "--port", dest="port", type='int', help="nvmf listen port", default=1158)
parser.add_option("-n", "--nsid", dest="nsid", type='int', help="namespace id", default=1)
parser.add_option("-s", "--subnqn",dest="subnqn", help="nvmf subsystem nqn", default="nqn.2019-04.ibof\:subsystem1")
(options, args) = parser.parse_args()
filename="trtype=" + options.transport + " adrfam=IPv4" + " traddr=" + options.ip + " trsvcid=" + str(options.port) + " subnqn="  + options.subnqn + " ns=" + str(options.nsid);
print(filename);

# set up for output file
host_name = os.uname()[1]
result_file_name = host_name + "_perf_output.csv"
columns = "io_size,block_size,q_depth,workload_mix,readwrite,cpus_allowed,run_time,run,read_iops,read_bw(kib/s), \
        read_avg_lat(us),read_min_lat(us),read_max_lat(us),write_iops,write_bw(kib/s),write_avg_lat(us), \
        write_min_lat(us),write_max_lat(us)"
with open(result_file_name, "w+") as result_file:
    result_file.write(columns + "\n")

for i, (s, b, q, m, w, c, t) in enumerate(itertools.product(io_size_bytes, block_size, q_depth, mix, readwrite, cpus_allowed, run_time)):
    run_fio(filename, s, b, q, m, c, i, w, t)
    if profile_mode == True:
        parse_results(s, b, q, m, c, i, w, t)

result_file.close()
