#include "OfflineWaveform.h"

bool OfflineWaveform::isChDataHead(int offset)
{
    return (frameData.at(offset + 0) == 0xeb && frameData.at(offset + 1) == 0x90 && frameData.at(offset + 2) == 0xa5 && frameData.at(offset + 3) == 0x5a);
}

/**
 * @brief 计算某次采样数据中的通道个数
 * @param offset
 * @return 
 */
int OfflineWaveform::getChNumber(int offset)
{
    int number;
    if(frameData.at(offset + 0) == 0 && frameData.at(offset + 1) == 0)
        number = 0;
    else if(frameData.at(offset + 0) == 0x0f && frameData.at(offset + 1) == 0x0f)
        number = 1;
    else if(frameData.at(offset + 0) == 0xf0 && frameData.at(offset + 1) == 0xf0)
        number = 2;
    else
        number = 3;
    return number;
}

void OfflineWaveform::setWaveFile(QString &file)
{
    waveFile = file;
}

/**
 * @brief 获得某次采样的完整数据
 * @param number
 * @return 
 */
QVector<quint8> OfflineWaveform::getFrameData(qint32 number)
{
    frameData.clear();
    QFile file(waveFile);
    if(file.open(QIODevice::ReadOnly))
    {
        file.seek(frameStartPos[number]);
        int   len  = frameStartPos[number + 1] - frameStartPos[number];
        char *buff = new char[len + 1];
        file.read(buff, len);
        for(int i = 0; i < len; i++)
        {
            frameData.append(buff[i]);
        }
        delete[] buff;
        return frameData;
    }
}

int OfflineWaveform::getWaveform(QVector<ChInfo> &allCh)
{
    int    ret = 0;
    ChInfo ch;
    int    offset = 88;

    while(offset < frameData.size())
    {
        if(isChDataHead(offset))
            offset += 4;
        else
            return ret;

        ch.number = getChNumber(offset);
        offset += 8;

        int start_pos = (frameData.at(offset) << 8) + frameData.at(offset + 1);
        offset += 2;
        int len = (frameData.at(offset) << 8) + frameData.at(offset + 1);
        offset += 2;

        for(int i = 0; i < len; i++)
        {
            ch.key.append(i + start_pos);
            ch.value.append((frameData.at(offset + 0) << 8) + frameData.at(offset + 1));
            offset += 2;
        }
        allCh.append(ch);
        ch.key.clear();
        ch.value.clear();
    }

    return ret;
}

/**
 * @brief 计算文件中有多少次完整的采样数据
 * 找到数据中0x01234567的位置，并记录下来，算作采样数据的开头
 * @return 
 */
qint32 OfflineWaveform::getADsampleNumber()
{
    QFile file(waveFile);
    file.open(QIODevice::ReadOnly);
    int  length       = 0;
    char buffer[4]    = {0};
    sampleFrameNumber = 0;
    qint32 offset     = 0;
    while((length = file.read(buffer, 4)) != 0)
    {
        if(buffer[0] == 0x01 && buffer[1] == 0x23 && buffer[2] == 0x45 && buffer[3] == 0x67)
        {
            frameStartPos.push_back(offset);
            if(sampleFrameNumber++ % 3000 == 0)
            {
                emit sendSampleFrameNumber(sampleFrameNumber);
            }
        }
        offset += 4;
    }
    emit sendSampleFrameNumber(sampleFrameNumber);
    file.close();
    emit finishSampleFrameNumber();
    return 0;
}
