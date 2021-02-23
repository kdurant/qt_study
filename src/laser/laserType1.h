#ifndef LASERTYPE1_H
#define LASERTYPE1_H
// 海洋雷达激光控制器
#include "LaserController.h"

//    1	内触发选择（关光状态下选择）	55 AA 01 01 00 01 33 CC	  55 AA 01 01 00 01 33 CC
//    2	外触发选择（关光状态下选择）	55 AA 01 01 00 02 33 CC	  55 AA 01 01 00 02 33 CC
//    3	出光	  55 AA 01 02 00 01 33 CC	55 AA 01 02 00 01 33 CC
//    4	关光	  55 AA 01 02 00 02 33 CC	55 AA 01 02 00 02 33 CC
//    5	LD2延时设置	55 AA 01 07 XX XX 33 CC	  55 AA 01 07 XX XX 33 CC
//    6	TEC1温度设置	55 AA 02 01 XX XX 33 CC	  55 AA 02 01 XX XX 33 CC
//    7	TEC2温度设置	55 AA 02 02 XX XX 33 CC   55 AA 02 02 XX XX 33 CC
class LaserType1 : public LaserController
{
    LaserType1() = default;

public:
    bool setMode(OpenMode mode) const override
    {
        quint8 data[8] = {0x55, 0xAA, 0x01, 0x01, 0x00, 0x01, 0x33, 0xcc};
        QByteArray array;
        for (int i = 0; i < 8; i++) {
            array.append(data[i] & 0xff);
        }
        array[5] = mode;
        return true;
    }

    bool open(void) const override
    {
        quint8 data[8] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x01, 0x33, 0xcc};
        QByteArray array;
        for (int i = 0; i < 8; i++) {
            array.append(data[i] & 0xff);
        }
        return true;
    };

    bool close(void) const override
    {
        quint8 data[8] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x02, 0x33, 0xcc};
        QByteArray array;
        for (int i = 0; i < 8; i++) {
            array.append(data[i] & 0xff);
        }
        return true;
    };
    bool setPower(qint8 power) const override
    {
        QByteArray array;
        array.append((char) 0x55);
        array.append((char) 0xAA);
        array.append((char) 0x01);
        array.append((char) 0x07);
        array.append((char) 0x00);

        switch (power) {
        case 0:
            array.append((char) 0x64);
            break;
        case 1:
            array.append((char) 0x32);
            break;
        case 2:
            array.append((char) 0x00);
            break;
        default:
            break;
        }

        array.append((char) 0x33);
        array.append((char) 0xCC);

        return true;
    }

    QByteArray setLD2DlyTime(quint16 value)
    {
        quint8 data[4] = {0x55, 0xAA, 0x01, 0x07};
        QByteArray array;
        for (int i = 0; i < 4; i++) {
            array.append(data[i] & 0xff);
        }
        array.append((quint8)((0xff00 & value) >> 8));
        array.append((quint8)(0x00ff & value));
        array.append(0x33);
        array.append(0xcc);
        return array;
    }
};

#endif
