#include "RadarProtocolBasic.h"

/**
 * @brief 命令数据和采样波形数据分开存放
 * processFlag[0] = 1时，处理命令数据
 * processFlag[1] = 1时，处理采样数据
 * @param originFrame
 */
void RadarProtocolBasic::setDataFrame(QByteArray &originFrame)
{
    qint32 number;

    if(originFrame.mid(FrameField::COMMAND_POS, FrameField::COMMAND_LEN) == QByteArray::fromHex("80000006"))
    {
        originWaveFrame.push_back(originFrame);

        number = originFrame.mid(FrameField::PCK_NUM_POS, FrameField::PCK_NUM_LEN).toHex().toInt(nullptr, 16);
        if(number == 0 && waveCntLast > 0)
        {
            waveFrameCnt++;  // 记录已经存放了几个完整的预览数据
            waveCntLast = 0;
        }
        else
            waveCntLast++;
    }
    else
    {
        commandFrame = originFrame;
        hasCommandFrame |= 0x01;
    }
}

int RadarProtocolBasic::getWaveFrameCnt()
{
    return waveFrameCnt;
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
    if(hasCommandFrame & 0x01)
    {
        res.cmdData     = commandFrame.mid(FrameField::COMMAND_POS, FrameField::COMMAND_LEN);
        res.sys_para    = commandFrame.mid(272, 8);
        hasCommandFrame = 0x00;
    }
    else
    {
        res.cmdData = 0x00;
    }

    return res;
}
