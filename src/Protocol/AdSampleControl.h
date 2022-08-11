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
    void setTrgMode(int data)
    {
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::TRG_MODE, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setFirstPos(int data)
    {
        data             = (data >> 3) << 3;
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::FIRST_POS, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setFirstLen(int data)
    {
        data             = (data >> 3) << 3;
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::FIRST_LEN, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setSecondPos(int data)
    {
        data             = (data >> 3) << 3;
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::SECOND_POS, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setSecondLen(int data)
    {
        data             = (data >> 3) << 3;
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
        data             = (data >> 3) << 3;
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::COMPRESS_LEN, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setCompressRatio(int data)
    {
        data             = (data >> 3) << 3;
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::COMPRESS_RATIO, 4, frame);
        sleepWithoutBlock(interval);
    }

    void setSampleDelay(int data)
    {
        data             = (data >> 3) << 3;
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::SAMPLE_DELAY_TIME, 4, frame);
        sleepWithoutBlock(interval);
    }

    void setPmtDelayAndGateTime(quint16 delay, quint16 high)
    {
        quint32    data  = ((delay >> 3) << 16) + (high >> 3);
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::PMT_CONTROL_TIME, 4, frame);
        sleepWithoutBlock(interval);
    }

    void enabelADSample(quint32 status)
    {
        QByteArray frame = BspConfig::int2ba(status);
        emit       sendDataReady(MasterSet::PREVIEW_ENABLE, 4, frame);
        sleepWithoutBlock(interval);
    }

    void setAlgoAPos(int data)
    {
        data             = (data >> 3) << 3;
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::ALGO_A_POS, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setAlgoALen(int data)
    {
        data             = (data >> 3) << 3;
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::ALGO_A_LEN, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setAlgoBPos(int data)
    {
        data             = (data >> 3) << 3;
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::ALGO_B_POS, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setAlgoBSumThre(int data)
    {
        data             = (data >> 3) << 3;
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::ALGO_B_SUM_THRE, 4, frame);
        sleepWithoutBlock(interval);
    }
    void setAlgoBValueThre(int data)
    {
        data             = (data >> 3) << 3;
        QByteArray frame = BspConfig::int2ba(data);
        emit       sendDataReady(MasterSet::ALGO_B_VALUE_THRE, 4, frame);
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
