#ifndef DACONTROL_H
#define DACONTROL_H

#include "bsp_config.h"
#include "protocol.h"

class DAControl : public QObject
{
    Q_OBJECT

public:
    DAControl() = default;
    bool setChannalValue(quint32 channal, quint32 value)
    {
        QByteArray frame = BspConfig::int2ba(channal);
        frame.append(BspConfig::int2ba(value));
        emit sendDataReady(MasterSet::DA_SET_VALUE, frame.length(), frame);

        return true;
    }

signals:

    void sendDataReady(qint32 command, qint32 data_len, QByteArray& data);  // 需要发送的数据已经准备好

    void responseDataReady(void);  // 接收到响应数据
};
#endif
