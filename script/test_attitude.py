# coding=utf-8
import pyshark
import socket
import binascii
import time

address = ('127.0.0.1', 6666)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

cap = pyshark.FileCapture('laserOpen.pcapng')

validUdpPacket = []
"""
把需要的数据帧先放到队列里
"""
for i in cap:
    #if len(i) == 2:  # [<ETH Layer>, <ARP Layer>]
    #continue

    if len(i.layers) == 4 and str(i.layers[3]) == 'DATA':
        validUdpPacket.append(i.data.data)
    #else:
    #print("hello")

for frame in validUdpPacket:
    msg = str(frame)
    msg = binascii.a2b_hex(msg)
    s.sendto(msg, address)
    time.sleep(100)

s.close()
