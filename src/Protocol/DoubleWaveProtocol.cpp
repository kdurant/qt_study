#include "DoubleWaveProtocol.h"

QByteArray& DoubleWaveProtocol::removeChNeedlessInfo(QByteArray& data)
{
    qint32 firstLen = data.mid(8, 2).toHex().toInt(nullptr, 16);

    data = data.mid(10);           // 去掉 帧头(4)，通道号(2)，第一段起始位置(2)，第一段长度(2)
    data.remove(firstLen * 2, 4);  // 去掉 第二段起始位置(2)，第二段长度(2)
    return data;
}

WaveData DoubleWaveProtocol::getSignalWave()
{
    WaveData data;
    if(waveFrameCnt == 0)
    {
        data.isEmpty = true;
        return data;
    }

    qint32     last          = 0;
    qint32     cur           = 0;
    qint32     dataLen       = 0;
    qint32     compressLen   = 0;
    qint32     compressRatio = 0;
    QByteArray all_ch;
    QByteArray ch0, ch1, ch2, ch3;

    while(originWaveFrame.isEmpty() == false)  // 队列里有数据
    {
        cur     = originWaveFrame.head().mid(PCK_NUM_POS, PCK_NUM_LEN).toHex().toInt(nullptr, 16);
        dataLen = originWaveFrame.head().mid(VALID_DATA_LEN_POS, VALID_DATA_LEN_LEN).toHex().toInt(nullptr, 16);
        if((cur == 0) && (last > 0))  // 一帧数据已经全部找到
        {
            waveFrameCnt--;
            cur  = 0;
            last = 0;
            break;
        }
        else
        {
            all_ch.append(originWaveFrame.dequeue().mid(24, dataLen));
            last++;
        }
    }
    compressLen   = all_ch.mid(84, 2).toHex().toInt(nullptr, 16);
    compressRatio = all_ch.mid(86, 2).toHex().toInt(nullptr, 16);
    all_ch        = all_ch.mid(88);  // 丢掉前面的GPS等信息

    qint32 size = all_ch.size() / 4;
    ch0         = all_ch.mid(size * 0, size);
    ch1         = all_ch.mid(size * 1, size);
    ch2         = all_ch.mid(size * 2, size);
    ch3         = all_ch.mid(size * 3, size);

    qint32 firstPos  = 0;
    qint32 firstLen  = 0;
    qint32 secondPos = 0;
    qint32 secondLen = 0;

    firstPos  = ch0.mid(6, 2).toHex().toInt(nullptr, 16);
    firstLen  = ch0.mid(8, 2).toHex().toInt(nullptr, 16);
    secondPos = ch0.mid(10 + firstLen * 2, 2).toHex().toInt(nullptr, 16);
    secondLen = ch0.mid(12 + firstLen * 2, 2).toHex().toInt(nullptr, 16);

    data.ch[0].Data = removeChNeedlessInfo(ch0);
    data.ch[1].Data = removeChNeedlessInfo(ch1);
    data.ch[2].Data = removeChNeedlessInfo(ch2);
    data.ch[3].Data = removeChNeedlessInfo(ch3);

    for(int i = 0; i < firstLen + compressLen / 8 + (secondLen - compressLen); i++)
    {
        for(int j = 0; j < 4; j++)
        {
            if(i < firstLen)
            {
                data.ch[j].Coor.append(i + firstPos);
            }
            else
            {
                data.ch[j].Coor.append(i - firstLen + secondPos);
            }
        }
    }
    data.isEmpty = false;
    return data;
}
