# coding=utf-8
# python test_pcapng.py --file underWater_laserUp.pcapng
import pyshark
import argparse
import binascii
import socket
import time

address = ('127.0.0.1', 6666)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

parser = argparse.ArgumentParser(description="read udp packet from *.pcapng file and send to PC")
parser.add_argument("--file", help="Files to be parsed")
parser.add_argument("-i", "--interval", type=int, default=100, help="interval time of udp packet")
parser.add_argument("-c",
                    "--cmd",
                    type=str,
                    default='all',
                    choices=['all', 'laser', 'motor', 'preview', 'status'],
                    help="select which kind of data to upload")

args = parser.parse_args()

# args.file = 'underWater_laserUp.pcapng'
#
cap = pyshark.FileCapture(args.file, display_filter="udp")

for pck in cap:
    if str(pck.highest_layer) == 'DATA' and \
        pck.ip.dst == "192.168.1.166":
        raw_data = pck.udp.payload.raw_value

        flag = ""
        if args.cmd == 'all':
            flag = "800000"
        elif args.cmd == 'laser':
            flag = "80000003"
        elif args.cmd == 'status':
            flag = "80000003"
        elif args.cmd == 'preview':
            flag = "80000006"
        elif args.cmd == 'motor':
            flag = "8000000c"

        if raw_data.find(flag) > 0:
            print(raw_data)
            msg = binascii.a2b_hex(raw_data)
            s.sendto(msg, address)
            time.sleep(args.interval / 1000)
