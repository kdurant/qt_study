import socket
import binascii

address = ('127.0.0.1', 6666)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

#while True:
    #msg = raw_input()
    #if not msg:
        #break

msg = "aa555aa5aa555aa5000000010000001300000000000000040000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000019"


# FPGA上传设备参数
msg = "aa555aa5aa555aa50000017f800000050000000000000054000000c80000ea60000000c80000000f00000000000000000000000000000000010100331122334400000000000000000000000000000000000000000000002000000380000003e80000dea8000007d0000000c80000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000646f7562312e303027c2b5a8"

msg = binascii.a2b_hex(msg)
print(msg)
s.sendto(msg, address)


s.close()

