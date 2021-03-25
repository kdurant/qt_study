#ifndef PREVIEW_PROCESS_H
#define PREVIEW_PROCESS_H
#include "ProtocolDispatch.h"
#include "bsp_config.h"
#include "protocol.h"
#include <QtCore>

/*
 * 设置AD采样参数
 */
class AdSampleControll : public QObject
{
    Q_OBJECT
public:
    AdSampleControll()
    {
    }
    void setPreviewEnable(int data)
    {
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::PREVIEW_ENABLE, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setTotalSampleLen(int data)
    {
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::SAMPLE_LEN, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setPreviewRatio(int data)
    {
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::PREVIEW_RATIO, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setFirstPos(int data)
    {
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::FIRST_POS, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setFirstLen(int data)
    {
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::FIRST_LEN, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setSecondPos(int data)
    {
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::SECOND_POS, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setSecondLen(int data)
    {
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::SECOND_LEN, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setSumThreshold(int data)
    {
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::SUM_THRE, 4, frame);
        sleepWithoutBlock(interval);
    }

    void setValueThreshold(int data)
    {
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::VALUE_THRE, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setCompressLen(int data)
    {
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::COMPRESS_LEN, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setCompressRatio(int data)
    {
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::COMPRESS_RATIO, 4, frame);
        sleepWithoutBlock(interval);
    }

    void enabelADSample(quint32 status)
    {
        QByteArray frame = BspConfig::int2ba(status);
        emit       sendDataReady(MasterSet::PREVIEW_ENABLE, 4, frame);
        sleepWithoutBlock(interval);
    }

signals:
    void sendDataReady(qint32 command, qint32 data_len, QByteArray &data);

private:
    qint32 interval{20};

    void sleepWithoutBlock(qint32 interval)
    {
        QEventLoop waitLoop;
        QTimer::singleShot(interval, &waitLoop, &QEventLoop::quit);
        waitLoop.exec();
    }
};
#endif
