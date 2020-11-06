# coding=utf-8
import socket
import binascii


address = ('127.0.0.1', 5555)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(address)
while 1:
    data,addr=s.recvfrom(2048)  
    print(data)
