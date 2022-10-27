"""
从硬盘里读取数据，通过UDP,模拟采集上传数据

python test_ssd_data.py --file ~/Public/12345.bin
"""
import time
import argparse
import socket
import os
from udp_protocol import EncodeProtocol

parser = argparse.ArgumentParser(description="get ssd data and send to PC by UDP protocol")
parser.add_argument("--file", help="Files to be parsed")
parser.add_argument("-i", "--interval", type=int, default=100, help="interval time of udp packet")
args = parser.parse_args()

address = ('127.0.0.1', 6666)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

source_file_size = os.path.getsize(args.file)
readed_position = 0
target_s = b'\x01\x23\x45\x67\x89'


def read_in_chunks(f, chunk_size=2048):
    while True:
        data = f.read(chunk_size)  # 每次读取指定的长度
        if not data:
            break
        yield data


udp_frame = EncodeProtocol()


def send_data_to_pc(data):
    idx = 0
    while len(data) > 256:
        udp_frame.set_pck_num(idx)
        udp_frame.set_command(b'\x80\x00\x00\x06')
        udp_frame.set_data(data[:256])
        packet = udp_frame.get_frame()
        s.sendto(packet, address)
        time.sleep(args.interval / 1000)
        data = data[256:]
        idx = idx + 1

    udp_frame.set_pck_num(idx)
    udp_frame.set_data(data)
    packet = udp_frame.get_frame()
    s.sendto(packet, address)
    time.sleep(args.interval / 1000)


STATE = 0

block_start = 0
block_stop = 0

with open(args.file, 'rb') as ssd_data:
    chunk_size = 256
    read_data_size = 0
    for chuck in read_in_chunks(ssd_data, chunk_size):
        read_data_size = len(chuck)
        readed_position += read_data_size
        head_pos = chuck.find(target_s)

        if STATE == 0:
            if head_pos == -1:  # 没有找到数据头, 继续找
                continue
            else:
                p = ssd_data.tell()
                STATE = 1
                block_start = p - (read_data_size - head_pos)
                if head_pos + 88 > chunk_size:  # 剩余的数据不能找到完整的头信息
                    ssd_data.seek(-100, 1)
                    continue
        elif STATE == 1:  # 找下一个数据头, 两次数据头之间的数据是一次完整的采集数据
            if head_pos == -1:  # 没有找到数据头, 继续找
                continue
            else:
                p = ssd_data.tell()
                STATE = 0
                block_stop = p - (read_data_size - head_pos)
                ssd_data.seek(block_start, 0)
                print("\rprogress:{:0>12d}/{:0>12d}".format(readed_position, source_file_size), end='')
                send_data_to_pc(ssd_data.read(block_stop - block_start))
print("\n")
