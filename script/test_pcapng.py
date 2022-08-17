# coding=utf-8
# python test_pcapng.py --file underWater_laserUp.pcapng
import pyshark
import argparse
import binascii
import socket
import time
import tqdm

address = ('127.0.0.1', 6666)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

parser = argparse.ArgumentParser(description="read udp packet from *.pcapng file and send to PC")
parser.add_argument("--file", help="Files to be parsed")
parser.add_argument("-i", "--interval", type=int, default=100, help="interval time of udp packet")
args = parser.parse_args()
args.file = 'underWater_laserUp.pcapng'

cap = pyshark.FileCapture(args.file, display_filter="udp")

for pck in tqdm.tqdm(cap):
    if str(pck.highest_layer) == 'DATA' and \
        pck.ip.dst == "192.168.1.166" and \
        len(pck.data.data) == 284 * 2:
        #  print(pck.data.data)
        msg = binascii.a2b_hex(pck.data.data)
        s.sendto(msg, address)
        time.sleep(args.interval / 1000)
