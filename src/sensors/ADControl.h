#ifndef ADCONTROL_H
#define ADCONTROL_H

#include "bsp_config.h"
#include "protocol.h"
#include "ProtocolDispatch.h"

class ADControl : public QObject
{
    Q_OBJECT

public:
    ADControl() = default;
    qint32 getChannalValue(quint32 channal)
    {
        if(channal >= 8)
            return -1;

        QByteArray frame = BspConfig::int2ba(channal);
        emit       sendDataReady(MasterSet::AD_GET_VALUE, frame.length(), frame);

        QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
        connect(this, &ADControl::responseDataReady, &waitLoop, &QEventLoop::quit);
        QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
        waitLoop.exec();

        quint32 lane = BspConfig::ba2int(recvData.mid(FrameField::DATA_POS, 4));
        if(lane >= 8)
            return -1;
        quint32 value = BspConfig::ba2int(recvData.mid(FrameField::DATA_POS + 4, 4));

        if(value >= 4096)
            return -1;

        return value;
    }

signals:

    void sendDataReady(qint32 command, qint32 data_len, QByteArray data);  // 需要发送的数据已经准备好

    void responseDataReady(void);  // 接收到响应数据

private:
    QByteArray recvData;

public slots:
    void setNewData(QByteArray& data)
    {
        recvData = data;
        emit responseDataReady();
    }
};
#endif
