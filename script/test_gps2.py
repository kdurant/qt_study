"""
发送不同类型的GPS的固定数据帧
"""

import argparse
import socket
from struct import pack
from udp_protocol import EncodeProtocol

parser = argparse.ArgumentParser(description="send gps frame to PC by UDP protocol")
parser.add_argument("-t",
                    "--type",
                    required=True,
                    choices=['novatel', 'applanix', 'xw_gi7660', 'xw_gi5610'],
                    help="set gps type")
args = parser.parse_args()

address = ('127.0.0.1', 6666)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

gps_xw_gi7660 = "AA 55 03 11 22 03 04 05 06 c3 f5 48 40 3f 35 1e 3f b6 f3 2d 40 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 FF"
gps_xw_gi5610 = "AA 76 35 BC 08 A8 52 DB 10 D5 9E B1 12 9D 26 44 48 A5 A7 00 00 4E 62 7B FF F3 FF 38 00 00 00 F5 FF FF FF FD FF FF FF B6 F8 34 BE 43 E5 DF 3C 62 65 24 BE E8 86 26 3B C9 75 D3 BC E0 D9 7E 3F 07 00 20 00 D4"

gps_applanix = "02 28 40 6D 5A 00 00 31 68 08 64 1A 39 26 68 02 02 40 3F 5C F0 C9 81 1F C2 40 5E 4F 88 A7 38 DE 27 40 30 64 B4 95 54 E1 A5 BC 6A 55 6D BA AB E1 68 BC 81 F0 83 3C 6B 51 0A 3F EB 21 DB 5F 04 8B B3 BF A8 EA 60 D2 49 1C 4E 40 6B 61 13 15 75 84 FE 40 67 27 A2 4A 7E 17 DE 3C D3 96 45 BE 4B ED 02 BE 06 D6 64 3C 4D AB 5E 3B BC 91 B7 BC C7 1B 2F 7A 03"


def str2byte(s):
    frame = b''
    for i in s.split(' '):
        frame += int(i, 16).to_bytes(1, "big")
    return frame


frame = b''
if args.type == "novatel":
    pass
elif args.type == "applanix":
    frame = str2byte(gps_applanix)
elif args.type == "xw_gi7660":
    frame = str2byte(gps_xw_gi7660)
elif args.type == "xw_gi5610":
    frame = str2byte(gps_xw_gi5610)

udp_frame = EncodeProtocol()
udp_frame.set_command(b'\x80\x00\x00\02')
udp_frame.set_data(frame)
packet = udp_frame.get_frame()

s.sendto(packet, address)
