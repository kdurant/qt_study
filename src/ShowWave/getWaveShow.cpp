#include "getWaveShow.h"

bool WaveShow::isChDataHead(int offset)
{
    return (frameData.at(offset + 0) == 0xeb && frameData.at(offset + 1) == 0x90
            && frameData.at(offset + 2) == 0xa5 && frameData.at(offset + 3) == 0x5a);
}

int WaveShow::getChNumber(int offset)
{
    int number;
    if (frameData.at(offset + 0) == 0 && frameData.at(offset + 1) == 0)
        number = 0;
    else if (frameData.at(offset + 0) == 0x0f && frameData.at(offset + 1) == 0x0f)
        number = 1;
    else if (frameData.at(offset + 0) == 0xf0 && frameData.at(offset + 1) == 0xf0)
        number = 2;
    else
        number = 3;
    return number;
}

void WaveShow::setWaveFile(QString &file)
{
    waveFile = file;
}

QVector<quint8> WaveShow::getFrameData(qint32 number)
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

int WaveShow::getChData(QVector<ChInfo> &allCh)
{
    int ret = 0;
    ChInfo ch;
    int offset = 88;

    while (offset < frameData.size()) {
        if (isChDataHead(offset))
            offset += 4;
        else
            return ret;

        ch.number = getChNumber(offset);
        offset += 8;

        int start_pos = (frameData.at(offset) << 8) + frameData.at(offset + 1);
        offset += 2;
        int len = (frameData.at(offset) << 8) + frameData.at(offset + 1);
        offset += 2;

        for (int i = 0; i < len; i++) {
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

qint32 WaveShow::getFrameNumber()
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
            if(sampleFrameNumber++ % 100000 == 0)
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
