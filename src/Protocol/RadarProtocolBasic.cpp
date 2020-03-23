#include "RadarProtocolBasic.h"

RadarProtocolBasic::RadarProtocolBasic()
{
    head         = {0xAA, 0x55, 0x5A, 0xA5, 0xAA, 0x55, 0x5A, 0xA5};
    cmdNum       = 0;
    cmdData      = 0;
    packetNum    = 0;
    validDataLen = 0;
    data         = {256, 0};

    processFlag = 0;
    waveFlag    = false;
}

/**
 * @brief 命令数据和采样波形数据分开存放
 * processFlag[0] = 1时，处理命令数据
 * processFlag[1] = 1时，处理采样数据
 * @param originFrame
 */
void RadarProtocolBasic::setDataFrame(QByteArray &originFrame)
{
    QByteArray ba = originFrame.mid(COMMAND_POS, COMMAND_LEN);
    qDebug() << (ba == QByteArray::fromHex("00000013"));

    if(originFrame.mid(COMMAND_POS, COMMAND_LEN) == QByteArray::fromHex("80000006"))
    {
        waveFrame.push_back(originFrame);
        waveFlag = true;
    }
    else
    {
        commandFrame = originFrame;
        if(waveFlag)
            processFlag |= 0x02;
        processFlag |= 0x01;
    }
}

QByteArray RadarProtocolBasic::encode(qint32 command, qint32 data_len, qint32 data)
{
    QByteArray frame;
    QByteArray origin;
    qint32     checksum = 0xeeeeffff;
    origin.append("AA555AA5AA555AA5");
    origin.append(QByteArray::number(cmdNum++, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(command, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(packetNum, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(data_len, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(data, 16).rightJustified(8, '0').append(504, '0'));
    origin.append(QByteArray::number(checksum, 16).rightJustified(8, '0'));
    frame = QByteArray::fromHex(origin);

    return frame;
}
