#include "getWaveShow.h"

void WaveShow::setWaveFile(QString file)
{
    waveFile = file;
}

QVector<qint32> WaveShow::getFrameData(qint32 number)
{
    QVector<qint32> ret;
    QFile file(waveFile);
    if (file.open(QIODevice::ReadOnly)) {
        file.seek(frameStartPos[number]);
        int len = frameStartPos[number + 1] - frameStartPos[number];
        char *buff = new char[len + 1];
        file.read(buff, len);
        for (int i = 0; i < len; i++) {
            ret[i] = buff[i];
        }
        delete[] buff;
        return ret;
    }
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
