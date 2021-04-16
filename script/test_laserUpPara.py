# coding=utf-8
import pyshark
import socket
import binascii
import time

address = ('127.0.0.1', 6666)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

cap = pyshark.FileCapture('underWater_laserUp.pcapng')

validUdpPacket = []
"""
把需要的数据帧先放到队列里
"""
for i in cap:
    if str(i.highest_layer) == 'DATA' and i.data.data.find('80000003') == 24:
        validUdpPacket.append(i.data.data)
        #print(i.data.data)

#msg = binascii.a2b_hex(validUdpPacket[0])
#s.sendto(msg, address)

for frame in validUdpPacket:
    msg = binascii.a2b_hex(str(frame))
    s.sendto(msg, address)
    time.sleep(0.5)

#s.close()
