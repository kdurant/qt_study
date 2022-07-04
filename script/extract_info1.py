#-*- coding: UTF-8 -*-
import argparse
import matplotlib.pyplot as plt
import os
import struct

import logging

LOG_FORMAT = "%(asctime)s - %(levelname)s - %(message)s"
DATE_FORMAT = "%m/%d/%Y %H:%M:%S %p"
logging.basicConfig(filename='my.log', level=logging.CRITICAL, format=LOG_FORMAT, datefmt=DATE_FORMAT)

parser = argparse.ArgumentParser(description="get motor and gps info from origin data of radar")
parser.add_argument("file", help="Files to be parsed")
args = parser.parse_args()

source_file_size = os.path.getsize(args.file)
source_file = open(args.file, "rb")

target_s = b'\x01\x23\x45\x67\x89'

readed_position = 0
motor_info = []
gps_sub_time = []
gps_second = []

latitude = []
longitude = []
height = []

interval = 0


def read_in_chunks(f, chunk_size=2048):
    while True:
        data = f.read(chunk_size)  # 每次读取指定的长度
        if not data:
            break
        yield data


with open(args.file, 'rb') as f:
    chunk_size = 1024*2
    for chuck in read_in_chunks(f, chunk_size):
        readed_position += chunk_size
        head = chuck.find(target_s)
        if head == -1:  # 没有找到数据头
            continue
        if head + 88 > chunk_size:  # 剩余的数据不能找到完整的头信息
            f.seek(-100, 1)
            continue

        # 提取GPS秒
        target = chuck[head + 12:head + 12 + 8]
        tmp = struct.unpack('>d', target)[0]
        gps_second.append(tmp)

        # 提取GPS细分时间
        target = chuck[head + 20:head + 20 + 4]
        gps_sub_time.append(int.from_bytes(target, byteorder='big'))

        # 提取电机计数值
        target = chuck[head + 76:head + 76 + 4]
        motor_info.append(int.from_bytes(target, byteorder='big'))

        interval += 1
        if (interval % 1000) == 0:
            print("\rprogress:{:0>10d}/{:0>10d}".format(readed_position, source_file_size), end='')

print("\rprogress:{:0>10d}/{:0>10d}".format(readed_position, source_file_size), end='')

fig, ax = plt.subplots(3, 1, sharex=True)

ax[0].plot(gps_second, color='r')
#ax[0].set_title("motor postion")
ax[0].set_ylabel("gps second", fontsize=15)
ax[0].grid(True)

ax[1].plot(gps_sub_time)
#ax[1].set_title("gps time")
ax[1].set_xlabel("sample series", fontsize=15)
ax[1].set_ylabel("gps time", fontsize=15)
ax[1].grid(True)

ax[2].plot(motor_info)
#ax[1].set_title("gps time")
ax[2].set_ylabel("motor cnt", fontsize=15)
ax[2].grid(True)

plt.show()
