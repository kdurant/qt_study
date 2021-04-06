#include "OfflineWaveform.h"

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

/**
 * @brief 获得某次采样的电机计数值
 * @param number
 * @return
 */
quint32 OfflineWaveform::getMotorCnt(qint32 number)
{
    char  buf[4] = {0};
    QFile file(waveFile);
    if(file.open(QIODevice::ReadOnly))
    {
        file.seek(frameStartPos[number] + 76);
        file.read(buf, 4);
    }
    quint32 ret = (static_cast<quint8>(buf[0]) << 24) + (static_cast<quint8>(buf[1]) << 16) + (static_cast<quint8>(buf[2]) << 8) + (static_cast<quint8>(buf[3]) << 0);

    return ret;
}

/**
 * @brief 分析采样次数时，顺便提取的电机计数值
 * @return
 */
QVector<double> OfflineWaveform::getMotorCnt()
{
    return motorCnt;
}

QVector<double> OfflineWaveform::getGpsSubTime()
{
    return gpsSubTime;
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
    quint32 offset    = 0;
    while((length = file.read(buffer, 4)) != 0)
    {
        if(buffer[0] == 0x01 && buffer[1] == 0x23 && buffer[2] == 0x45 && buffer[3] == 0x67)
        {
            frameStartPos.push_back(offset);
            if(sampleFrameNumber++ % 3000 == 0)
            {
                emit sendSampleFrameNumber(sampleFrameNumber);
            }
            // 顺便读取GPS细分时间
            file.seek(offset + 20);
            file.read(buffer, 4);
            quint32 value = (static_cast<quint8>(buffer[0]) << 24) + (static_cast<quint8>(buffer[1]) << 16) + (static_cast<quint8>(buffer[2]) << 8) + (static_cast<quint8>(buffer[3]) << 0);
            gpsSubTime.append(value);

            // 顺便读取电机计数值
            file.seek(offset + 76);
            file.read(buffer, 4);
            offset += 76;
            value = (static_cast<quint8>(buffer[0]) << 24) + (static_cast<quint8>(buffer[1]) << 16) + (static_cast<quint8>(buffer[2]) << 8) + (static_cast<quint8>(buffer[3]) << 0);
            motorCnt.append(value);
        }
        offset += 4;
    }
    emit sendSampleFrameNumber(sampleFrameNumber);
    file.close();
    emit finishSampleFrameNumber();
    return 0;
}
