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

    /**
     * @brief run, 控制电机以指定速度(r/s)运行
     * @param speed
     * @return
     */
    virtual bool run(quint16 speed)
    {
        return false;
    }

    virtual qint32 getActualVelocity(void)
    {
        return 0;
    }
    virtual qint32 getActualPosition(void)
    {
        return 0;
    }

signals:
    void sendDataReady(QByteArray& data);  // 需要发送的数据已经准备好

    void sendDataReady(qint32 command, qint32 data_len, QByteArray& data);  // 需要发送的数据已经准备好

    void responseDataReady(void);  // 接收到响应数据
};
#endif
