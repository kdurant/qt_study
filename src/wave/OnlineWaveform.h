#ifndef ONLINE_WAVEFORM_H
#define ONLINE_WAVEFORM_H
#include <QtCore>

#include "ProtocolDispatch.h"
/*
 * 在使能采集后，采集卡会不断的上传采集数据
 * 本模块的作用是将采集数据流分界，获得一次采样的完整数据后，交给其他模块
 *
 * 1. udp数据帧中的包序号重新计数
 * 2. 指定时间内没有收到新的数据
 */

class OnlineWaveform : public QObject
{
    Q_OBJECT

private:
    bool       isRecvNewData;  // 是否收到数据
    QByteArray recvData;
    qint32     waitTime{10};

    bool       isStartRecvCompleteFrame{false};
    QByteArray fullSampleWave;
    qint32     curPckNumber{0};
    qint32     prePckNumber{0xffff};

    QVector<quint32> number;

public:
    OnlineWaveform()
    {
        isRecvNewData = false;
        //        connect(this, &OnlineWaveform::responseDataReady, this, &OnlineWaveform::getSampleData);
    }

    bool isFrameHead(QByteArray &data)
    {
        if(data.size() < 4)
            return false;
        return (data.at(0) == 0x01 && data.at(1) == 0x23 && data.at(2) == 0x45 && data.at(3) == 0x67);
    }
signals:
    void responseDataReady(void);                // 接收到响应数据
    void fullSampleDataReady(QByteArray &data);  // 找到一次完整的采样数据

public slots:
    void setNewData(QByteArray data)
    {
        isRecvNewData = true;
        recvData      = data;
        //        emit responseDataReady();
        getSampleData(recvData);
    }
    void getSampleData(QByteArray &frame);
};
#endif
