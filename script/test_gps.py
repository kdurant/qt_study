"""
从硬盘数据中提取的gps文件中读取gps数据，
模拟Novatel型号GPS 实际的数据帧，通过UDP发到上位机软件
usage: python test_gps.py --file gps_info.log -i 2000
"""

import time
import argparse
import socket
import struct
from udp_protocol import EncodeProtocol, NovatelFrame

parser = argparse.ArgumentParser(description="get gps info and send to PC by UDP protocol")
parser.add_argument("--file", help="Files to be parsed")
parser.add_argument("-i", "--interval", type=int, default=100, help="interval time of udp packet")
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

num = 0
with open(args.file, 'r') as gps_data:
    """
    string格式, 后面有'\n'
    4629521348589685941,4638232073943300187

    """
    line = gps_data.readline()
    while line:
        #  if (num > 2610 and num < 2800) or (num > 6070 and num < 6240):
        if num >= 0:
            line = line[:-1]

            # 维度      经度
            gps_sub_time, latitude, longitude, height, azimuth, pitch, roll = line.split(',')

            gps_sub_time = int(gps_sub_time)
            latitude = int(latitude)
            longitude = int(longitude)
            height = int(height)
            azimuth = int(azimuth)
            pitch = int(pitch)
            roll = int(roll)

            print("num: {:6d}/17806".format(num))

            gps_frame.set_second(gps_sub_time)
            gps_frame.set_latitude(latitude.to_bytes(8, byteorder='little'))
            gps_frame.set_longitude(longitude.to_bytes(8, byteorder='little'))
            gps_frame.set_height(height.to_bytes(8, byteorder='little'))
            gps_frame.set_azimuth(azimuth.to_bytes(8, byteorder='little'))
            gps_frame.set_pitch(pitch.to_bytes(8, byteorder='little'))
            gps_frame.set_roll(roll.to_bytes(8, byteorder='little'))

            sub_frame = gps_frame.get_frame()
            udp_frame.set_data(sub_frame)
            packet = udp_frame.get_frame()

            s.sendto(packet, address)
            time.sleep(args.interval / 1000)

            #  exit(0)latitudi
        num = num + 1
        line = gps_data.readline()
