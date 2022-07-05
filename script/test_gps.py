"""
从硬盘数据中提取的gps文件中读取gps数据，
模拟实际GPS数据，通过UDP发到上位机软件
"""

import argparse
import socket

parser = argparse.ArgumentParser(description="get gps info and send to PC by UDP protocol")
parser.add_argument("--file", help="Files to be parsed")
args = parser.parse_args()

address = ('127.0.0.1', 6666)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

#  gps_data = open("args.file")

with open(args.file, 'r') as gps_data:
    """
    string格式, 后面有'\n'
    4629521348589685941,4638232073943300187

    """
    line = gps_data.readline()
    line = line[:-1]

    # 维度      经度
    latitude, longitude = line.split(',')
    print(line)
