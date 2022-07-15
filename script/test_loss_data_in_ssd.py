#-*- coding: UTF-8 -*-
"""
从存储的硬盘数据文件中提取出GPS时间，电机计数值，检测保存数据是否有丢失
"""
FIELD_CONFG = {
    'head': {
        'len': 8,
        'offset': 0
    },
    'gps_week': {
        'len': 4,
        'offset': 8
    },
    'gps_second': {
        'len': 8,
        'offset': 12
    },
    'gps_sub_time': {
        'len': 4,
        'offset': 20
    },
    'azimuth': {
        'len': 8,
        'offset': 24
    },
    'pitch': {
        'len': 8,
        'offset': 32
    },
    'roll': {
        'len': 8,
        'offset': 40
    },
    'latitude': {
        'len': 8,
        'offset': 48
    },
    'longitude': {
        'len': 8,
        'offset': 56
    },
    'height': {
        'len': 8,
        'offset': 64
    },
    'motor_bits': {
        'len': 4,
        'offset': 72
    },
    'motor_cnt': {
        'len': 4,
        'offset': 76
    },
    'channal_num': {
        'len': 4,
        'offset': 80
    },
    'sys_info': {
        'len': 4,
        'offset': 84
    },
}

import argparse
from sys import byteorder
import matplotlib.pyplot as plt
import os
import struct

import logging

from numpy.lib.function_base import append

LOG_FORMAT = "%(asctime)s - %(levelname)s - %(message)s"
DATE_FORMAT = "%m/%d/%Y %H:%M:%S %p"
logging.basicConfig(filename='my.log', level=logging.CRITICAL, format=LOG_FORMAT, datefmt=DATE_FORMAT)

parser = argparse.ArgumentParser(description="get motor and gps info from origin data of radar")
parser.add_argument("--file", help="Files to be parsed")
parser.add_argument("--gps", action='store_true', help='analyze gps info')
args = parser.parse_args()

source_file_size = os.path.getsize(args.file)
source_file = open(args.file, "rb")

target_s = b'\x01\x23\x45\x67\x89'

readed_position = 0
motor_info = []
gps_sub_time = []
gps_second = []
gps_latitude = []
gps_longitude = []
gps_height = []
gps_azimuth = []  # 偏航角, "摇头", (heading)
gps_pitch = []  # 俯仰角, "点头"
gps_roll = []  # 翻滚角, "翻滚"

interval = 0


def read_in_chunks(f, chunk_size=2048):
    while True:
        data = f.read(chunk_size)  # 每次读取指定的长度
        if not data:
            break
        yield data


def get_field_bytes(chunk, head, key):
    offset = FIELD_CONFG[key]['offset']
    len = FIELD_CONFG[key]['len']
    target = chuck[head + offset:head + offset + len]
    return target


with open(args.file, 'rb') as f:
    chunk_size = 1024 * 2
    for chuck in read_in_chunks(f, chunk_size):
        readed_position += chunk_size
        head = chuck.find(target_s)
        if head == -1:  # 没有找到数据头
            continue
        if head + 88 > chunk_size:  # 剩余的数据不能找到完整的头信息
            f.seek(-100, 1)
            continue

        # 提取GPS秒
        target = get_field_bytes(chuck, head, 'gps_second')
        tmp = struct.unpack('>d', target)[0]
        gps_second.append(tmp)

        # 提取GPS细分时间
        target = get_field_bytes(chuck, head, 'gps_sub_time')
        gps_sub_time.append(int.from_bytes(target, byteorder='big'))

        # 提取电机计数值
        target = get_field_bytes(chuck, head, 'motor_cnt')
        motor_info.append(int.from_bytes(target, byteorder='big'))

        interval += 1
        if (interval % 1000) == 0:
            print("\rprogress:{:0>12d}/{:0>12d}".format(readed_position, source_file_size), end='')

print("\rprogress:{:0>12d}/{:0>12d}".format(readed_position, source_file_size), end='')

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
