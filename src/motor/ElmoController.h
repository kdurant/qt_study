#ifndef ELMO_H
#define ELMO_H
#include <QtCore>
#include "MotorController.h"

// 电机一圈计数 236800

class Elmo : public MotorController
{
public:
    Elmo() = default;

public:
    MOTOR_STATUS init(void) override;
    MOTOR_STATUS run(quint16 speed) override;

    qint32 getActualVelocity(void) override;
    qint32 getActualPosition(void) override;

    bool         moveToHome(void) override;
    MOTOR_STATUS moveToPosition(double postion, int direct) override;
    bool         moveFixSpeed(quint32 speed) override;

    bool sweep(quint32 start_pos, quint32 end_pos);

private:
    bool       isRecvNewData{false};  // 是否收到数据
    QByteArray recvData;
    qint32     waitTime{50};

    quint32 maxVelocity{3571};
    quint32 quickStopDeceleration{45};

    void waitResponse(quint16 waitMS)
    {
        QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
        connect(this, &Elmo::responseDataReady, &waitLoop, &QEventLoop::quit);
        QTimer::singleShot(waitMS, &waitLoop, &QEventLoop::quit);
        waitLoop.exec();
    }

public slots:

    void setNewData(QByteArray& data)
    {
        isRecvNewData = true;
        recvData      = data;
        emit responseDataReady();
    }
};

#endif
