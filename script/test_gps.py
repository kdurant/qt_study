"""
从硬盘数据中提取的gps文件中读取gps数据，
模拟实际GPS数据，通过UDP发到上位机软件
"""

import time
import argparse
import socket
from udp_protocol import EncodeProtocol, NovatelFrame

parser = argparse.ArgumentParser(description="get gps info and send to PC by UDP protocol")
parser.add_argument("--file", help="Files to be parsed")
args = parser.parse_args()

address = ('127.0.0.1', 6666)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

#  gps_data = open("args.file")

gps_frame = NovatelFrame()
udp_frame = EncodeProtocol()

gps_frame.set_latitude(b'\x01' * 8)
gps_frame.set_longitude(b'\x02' * 8)
sub_frame = gps_frame.get_frame()

udp_frame.set_data(sub_frame)
udp_frame.get_frame()
#  print(len(udp_frame.get_frame()))

with open(args.file, 'r') as gps_data:
    """
    string格式, 后面有'\n'
    4629521348589685941,4638232073943300187

    """
    line = gps_data.readline()
    line = line[:-1]

    # 维度      经度
    latitude, longitude, height, azimuth, pitch, roll = line.split(',')
    latitude = int(latitude)
    longitude = int(longitude)
    height = int(height)
    azimuth = int(azimuth)
    pitch = int(pitch)
    roll = int(roll)

    gps_frame.set_latitude(latitude.to_bytes(8, byteorder='little'))
    gps_frame.set_longitude(longitude.to_bytes(8, byteorder='little'))
    gps_frame.set_height(height.to_bytes(8, byteorder='little'))
    gps_frame.set_azimuth(azimuth.to_bytes(8, byteorder='little'))
    sub_frame = gps_frame.get_frame()
    udp_frame.set_data(sub_frame)
    packet = udp_frame.get_frame()

    print(len(packet))
    s.sendto(packet, address)
    #  time.sleep(0.5)
    exit(0)
