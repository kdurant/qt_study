#include "protocol.h"

Protocol::Protocol()
{
    cmd_num = 1;
}

Protocol::~Protocol()
{
}

QByteArray Protocol::encode(qint32 command, qint32 data_len, qint32 data)
{
    QByteArray frame;
    QByteArray origin;
    qint32 checksum = 0xeeeeffff;
    origin.append("AA555AA5AA555AA5");
    origin.append(QByteArray::number(cmd_num++, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(command, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(pck_num, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(data_len, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(data, 16).leftJustified(512, '0'));
    origin.append(QByteArray::number(checksum, 16).rightJustified(8, '0'));
    frame = QByteArray::fromHex(origin);

//    QByteArray frame("AA555AA5AA555AA5");
//    qint32 checksum = 0xeeeeffff;
//    frame.append(QByteArray::number(cmd_num++, 16).rightJustified(8, '0'));
//    frame.append(QByteArray::number(command, 16).rightJustified(8, '0'));
//    frame.append(QByteArray::number(pck_num, 16).rightJustified(8, '0'));
//    frame.append(QByteArray::number(data_len, 16).rightJustified(8, '0'));
//    frame.append(QByteArray::number(data, 16).leftJustified(256, '0'));
//    frame.append(QByteArray::number(checksum, 16).leftJustified(8, '0'));


//    frame.append((uchar)0xAA);
//    frame.append((uchar)0x55);
//    frame.append((uchar)0x5A);
//    frame.append((uchar)0xA5);
//    frame.append((uchar)0xAA);
//    frame.append((uchar)0x55);
//    frame.append((uchar)0x5A);
//    frame.append((uchar)0xA5);

//    frame.append((uchar)((cmd_num>>24)&0xff));
//    frame.append((uchar)((cmd_num>>16)&0xff));
//    frame.append((uchar)((cmd_num>> 8)&0xff));
//    frame.append((uchar)((cmd_num>> 0)&0xff));
//    cmd_num++;

//    frame.append((uchar)((command>>24)&0xff));
//    frame.append((uchar)((command>>16)&0xff));
//    frame.append((uchar)((command>> 8)&0xff));
//    frame.append((uchar)((command>> 0)&0xff));

//    frame.append((uchar)((pck_num>>24)&0xff));
//    frame.append((uchar)((pck_num>>16)&0xff));
//    frame.append((uchar)((pck_num>> 8)&0xff));
//    frame.append((uchar)((pck_num>> 0)&0xff));

    return frame;
}
