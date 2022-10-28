"""
产生陆地雷达的预览数据, 保存成文件，  ./test_ssd_data.py 发送给上位机测试

2. 正常数据，
"""
import binascii

head = "0123456789abcdef00000000000000000000000039c475c600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011000072ba0000000400001770"

# start: 0x20
# len:   0xc8
main_wave = "002000c800d700d400d700d700d800dd0154028b036f03bc03d603d903d503c2038802f2022401b60163011b00f400e400df00e000e800f400ff010c0115011901170115010b010100f600eb00e400de00d800d000d100d400d700df00e300ec00f300f800fb00fc00fa00f700f000eb00e700e200de00da00da00d800dc00dc00e200e500e900ee00ef00ef00f100ef00ec00e700e400e400e000de00d900d900dc00db00dc00df00e300e500e700e900e800e900eb00e800e800e700e700e300e000df00db00dd00dd00de00e000df00e000e100e300e400e400e400e400e500e400e100e000e100e000e100df00df00e000e200e200e300e300e300e000e100e000e200e200e200e100e000df00e100e000e000e100e000e300e400e400e400e300e100e100e000e100df00df00df00dd00dc00dd00dc00df00dd00df00e100e400e500e300e400e400e100e000df00e000e100de00df00de00de00df00e200e100e000df00e000e000e000df00e100e100e200e100df00e100e300e200e100e000de00df00df00e000e100e000e000de00dd"

###### 正常数据
MODE = 3
sub_wave1 = ""
sub_wave2 = ""
sub_wave3 = ""
sub_wave3 = ""

if MODE == 0:  #  有一个子段
    sub_wave1 = "0000012c012201220122012201220122012201220122012201220122012201220122012201220122012201220122012201220122"
    sub_wave2 = ""
    sub_wave3 = ""
    sub_wave3 = ""
elif MODE == 1:  # 子段尾部有冗余数据
    sub_wave1 = "0000012c012201220122012201220122012201220122012201220122012201220122012201220122012201220122012201220122"
    sub_wave2 = "11223344"
    sub_wave3 = ""
    sub_wave3 = ""
elif MODE == 2:  # 子段尾部少数据
    sub_wave1 = "0000012c0122012201220122012201220122012201220122012201220122012201220122012201220122012201220122"
    sub_wave2 = ""
    sub_wave3 = ""
    sub_wave3 = ""
elif MODE == 3:  # 有两个相邻子段
    sub_wave1 = "0000012c012201220122012201220122012201220122012201220122012201220122012201220122012201220122012201220122"
    sub_wave2 = "0000013c01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc01fc"
    sub_wave3 = ""
    sub_wave3 = ""

###### 数据尾部多数据

f = open("test.bin", "wb")
f.write(binascii.a2b_hex(head + main_wave + sub_wave1 + sub_wave2) * 4)
f.close()