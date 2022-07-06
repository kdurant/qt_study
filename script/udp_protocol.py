class EncodeProtocol():

    def __init__(self):
        self.synch_head = b'\xAA\x55\x5A\xA5\xAA\x55\x5A\xA5'
        self.cmd_num = (0).to_bytes(4, byteorder='big')
        self.command = b'\x80\x00\x00\x02'
        self.pck_num = (1).to_bytes(4, byteorder='big')
        self.checksum = b'\xaa\xbb\xcc\xdd'

    def set_data(self, data):
        self.data_len = len(data).to_bytes(4, byteorder='big')
        self.data = data + (256 - len(data)) * b'\x00'

    def get_frame(self):
        frame = self.synch_head + self.cmd_num + self.command + self.pck_num + self.data_len + self.data + self.checksum
        return frame

    def config(self,
               head='AA555AA5AA555AA5',
               cmd_num='00000000',
               command='00000000',
               pck_num='11223344',
               data_len=4,
               data='0000'):
        self.head = ''.join(head.split())
        self.cmd_num = ''.join(cmd_num.split()).zfill(8)
        self.command = command
        self.pck_num = ''.join(pck_num.split()).zfill(8)
        self.data_len = data_len.to_bytes(8, byteorder='big')
        self.data = ''.join(data.split()).zfill(int(data_len) * 2) + (512 - int(data_len) * 2) * '0'
        pass


import struct


class NovatelFrame():

    def __init__(self):
        self.synch_head = b'\x11\x11\x11\x11\x22\x22\x22\x22\x33\x33\x33\x33'
        self.week = 17476  # 0x44444
        self.second = 1234561.3
        self.latitude = self.double_to_hex(102.3)
        self.longitude = self.double_to_hex(23.4)
        self.height = self.double_to_hex(534.21)
        self.north_velocity = 11.1
        self.east_velocity = 22.2
        self.up_velocity = 33.3
        self.roll = self.double_to_hex(44.4)
        self.pitch = self.double_to_hex(55.5)
        self.azimuth = self.double_to_hex(66.6)
        self.status = 21845  # 0x5555
        self.padding = b'\x99' * 11  # 不清楚为什么上位机里的GPS数据长度是115
        self.crc = 26214  # 0x6666

    def double_to_hex(self, f):
        format_string = hex(struct.unpack('<Q', struct.pack('<d', f))[0])
        format_bytes = bytearray.fromhex(format_string.lstrip('0x').rstrip('L'))
        return format_bytes

    def set_latitude(self, latitude):
        self.latitude = latitude

    def set_longitude(self, longitude):
        self.longitude = longitude

    def set_height(self, height):
        self.height = height

    def set_azimuth(self, azimuth):
        self.azimuth = azimuth

    def set_pitch(self, pitch):
        self.pitch = pitch

    def set_roll(self, roll):
        self.roll = roll

    def get_frame(self):
        frame = self.synch_head                         + \
            self.week.to_bytes(4, byteorder='little')   + \
            self.double_to_hex(self.second)             + \
            self.latitude                               + \
            self.longitude                              + \
            self.height                                 + \
            self.double_to_hex(self.north_velocity)     + \
            self.double_to_hex(self.east_velocity)      + \
            self.double_to_hex(self.up_velocity)        + \
            self.roll                                   + \
            self.pitch                                  + \
            self.azimuth                                + \
            self.status.to_bytes(4, byteorder='little') + \
            self.padding                                + \
            self.crc.to_bytes(4, byteorder='little')

        return frame
