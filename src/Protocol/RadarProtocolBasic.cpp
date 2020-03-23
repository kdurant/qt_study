#include "RadarProtocolBasic.h"

/**
 * @brief 命令数据和采样波形数据分开存放
 * processFlag[0] = 1时，处理命令数据
 * processFlag[1] = 1时，处理采样数据
 * @param originFrame
 */
void RadarProtocolBasic::setDataFrame(QByteArray &originFrame)
{
    quint32 number;

    if(originFrame.mid(COMMAND_POS, COMMAND_LEN) == QByteArray::fromHex("80000006"))
    {
        waveFrame.push_back(originFrame);

        number = originFrame.mid(PCK_NUM_POS, PCK_NUM_LEN).toInt(nullptr, 16);
        waveCntCur++;
        if(waveCntCur == 0 && waveCntLast > 0)
            waveFrameCnt++;

        waveCntLast++;
    }
    else
    {
        commandFrame = originFrame;
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

ProtocolResult RadarProtocolBasic::getFPGAInfo()
{
    ProtocolResult res;
    if(processFlag & 0x01)
    {
        res.cmdData  = commandFrame.mid(COMMAND_POS, COMMAND_LEN);
        res.sys_para = commandFrame.mid(272, 8);
        processFlag  = 0x00;
    }

    return res;
}
