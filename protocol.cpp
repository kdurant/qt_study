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
    qint32     checksum = 0xeeeeffff;
    origin.append("AA555AA5AA555AA5");
    origin.append(QByteArray::number(cmd_num++, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(command, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(pck_num, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(data_len, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(data, 16).rightJustified(8, '0').append(504,'0'));
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

QString &Protocol::get_single_ad_data(QQueue<QString> &frame)
{
    QString data;
    qint32  expect_index   = 0;
    qint32  valid_data_len = 0;

    ad_single_data.clear();

    // 找到包序号为0的数据帧
    while(frame.head().mid(PCK_NUMER_POS, PCK_NUMBER_LEN).toInt(nullptr, 16) != 0 && frame.empty() != true)
    {
        frame.dequeue();
    }

    //
    while(frame.head().mid(PCK_NUMER_POS, PCK_NUMBER_LEN).toInt(nullptr, 16) == expect_index && frame.empty() != true)
    {
        data           = frame.dequeue();
        valid_data_len = data.mid(VALID_LEN_POS, VALID_LEN_LEN).toInt(nullptr, 16) * 2;
        ad_single_data += data.mid(AD_DATA_POS, valid_data_len);
        expect_index++;
    }
    qDebug() << "single all data: " << ad_single_data;
    return ad_single_data;
}

AD_Data &Protocol::get_channal_data(qint32 number)
{
    qint32 idx = 0;

    switch(number)
    {
        case 0:
            idx = ad_single_data.indexOf(CHANNAL_0_FLAG) + 12;
            break;
        case 1:
            idx = ad_single_data.indexOf(CHANNAL_1_FLAG) + 12;
            break;
        case 2:
            idx = ad_single_data.indexOf(CHANNAL_2_FLAG) + 12;
            break;
        case 3:
            idx = ad_single_data.indexOf(CHANNAL_3_FLAG) + 12;
            break;
    }
    ad_data[number].first_start_pos = ad_single_data.mid(idx, 4).toInt(nullptr, 16);
    idx += 4;

    ad_data[number].first_len = ad_single_data.mid(idx, 4).toInt(nullptr, 16);
    idx += 4;

    ad_data[number].first_data = ad_single_data.mid(idx, ad_data[number].first_len * 4);
    idx += ad_data[number].first_len * 4;

    if(ad_single_data.mid(idx, 8) == "eb90a55a")
    {
        // 计算第二段数据
        ad_data[number].second_start_pos = ad_single_data.mid(idx, 4).toInt(nullptr, 16);
        idx += 4;
    }
    qDebug() << "channal 0 first data: " << ad_data[0].first_data;
//    memset(&ad_data[0], 0x00, sizeof(ad_data[0]));
    return ad_data[number];
}
