#ifndef MOTORCONTROLL_H
#define MOTORCONTROLL_H
#include <QtCore>

#include "protocol.h"

class MontorController : public QObject
{
    Q_OBJECT
public:
    MontorController() = default;

    virtual bool init()
    {
        return false;
    };

    virtual bool start()
    {
        return false;
    };

    virtual bool stop()
    {
        return false;
    };

signals:
    void sendDataReady(QByteArray& data);  // 需要发送的数据已经准备好

    void sendDataReady(qint32 command, qint32 data_len, QByteArray& data);  // 需要发送的数据已经准备好

    void responseDataReady(void);  // 接收到响应数据
};
#endif
