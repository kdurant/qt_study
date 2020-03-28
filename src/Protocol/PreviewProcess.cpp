#include "PreviewProcess.h"
#include "ProtocolDispatch.h"

void PreviewProcess::setDataFrame(QByteArray& frame)
{
    qint32 number;

    originFrame.push_back(frame);

    number = ProtocolDispatch::getPckNum(frame);
    if(number == 0 && pckNumLast > 0)
    {
        fullFrameCnt++;  // 记录已经存放了几个完整的预览数据
        pckNumLast = 0;
        emit previewReadyShow();
    }
    else
        pckNumLast++;
}

WaveData PreviewProcess::getSignalWave()
{
    WaveData wave;
    if(fullFrameCnt == 0)
    {
        return wave;
    }

    qint32     last          = 0;
    qint32     cur           = 0;
    qint32     dataLen       = 0;
    qint32     compressLen   = 0;
    qint32     compressRatio = 0;
    QByteArray all_ch;
    QByteArray ch0, ch1, ch2, ch3;

    while(originFrame.isEmpty() == false)  // 队列里有数据
    {
        cur     = ProtocolDispatch::getPckNum(originFrame.head());
        dataLen = ProtocolDispatch::getDataLen(originFrame.head());
        if((cur == 0) && (last > 0))  // 一帧数据已经全部找到
        {
            fullFrameCnt--;
            cur  = 0;
            last = 0;
            break;
        }
        else
        {
            all_ch.append(originFrame.dequeue().mid(24, dataLen));
            last++;
        }
    }
    qDebug() << "all data size is : " << all_ch.size();
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

    auto removeChNeedlessInfo = [](QByteArray& data) -> QByteArray {
        qint32 firstLen = data.mid(8, 2).toHex().toInt(nullptr, 16);
        data            = data.mid(10);  // 去掉 帧头(4)，通道号(2)，第一段起始位置(2)，第一段长度(2)
        data.remove(firstLen * 2, 4);    // 去掉 第二段起始位置(2)，第二段长度(2)
        return data;
    };

    wave.ch[0].data = removeChNeedlessInfo(ch0);
    wave.ch[1].data = removeChNeedlessInfo(ch1);
    wave.ch[2].data = removeChNeedlessInfo(ch2);
    wave.ch[3].data = removeChNeedlessInfo(ch3);

    for(int i = 0; i < firstLen + compressLen / compressRatio + (secondLen - compressLen); i++)
    {
        for(int j = 0; j < 4; j++)
        {
            if(i < firstLen)
            {
                wave.ch[j].coor.append(i + firstPos);
            }
            else
            {
                wave.ch[j].coor.append(i - firstLen + secondPos);
            }
        }
    }
    return wave;
}
