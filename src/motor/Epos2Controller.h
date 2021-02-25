#ifndef EPOS2_H
#define EPOS2_H
#include <QtCore>
#include "MotorController.h"

class EPOS2 : public MontorController
{
public:
    explicit EPOS2(QObject *parent = 0);

public:
    bool start(void) override;
    bool stop(void) override;

private:
    bool       isRecvNewData;  // 是否收到数据
    QByteArray recvData;

    void waitResponse(quint16 waitMS)
    {
        QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
        connect(this, &EPOS2::responseDataReady, &waitLoop, &QEventLoop::quit);
        QTimer::singleShot(waitMS, &waitLoop, &QEventLoop::quit);
        waitLoop.exec();
    }

    bool isResponse4f(void)
    {
        if (isRecvNewData) {
            isRecvNewData = false;
            return recvData.length() == 1 && recvData.at(0) == 0x4f;
        }
        return false;
    }
    bool isResponse4f00(void)
    {
        if (isRecvNewData) {
            isRecvNewData = false;
            return recvData.length() == 2 && recvData.at(0) == 0x4f && recvData.at(1) == 0x00;
        }
        return false;
    }

    bool send_4f_actively(void)
    {
        QByteArray frame(1, 0x4f);
        emit sendDataReady(MasterSet::MOTOR_PENETRATE, 1, frame);
    }

public:
    quint16          calcFieldCRC(quint16 *pDataArray, quint16 numberofWords);  //CRC-CCITT
    QVector<quint16> WordPlusCRC(QVector<quint16> word);
    QByteArray       transmitWord2Byte(QVector<quint16> word);
    QVector<quint16> receiveByte2Word(QByteArray array);

    QByteArray setDisableState();
    QByteArray clearFault();
    QByteArray setShutdown();
    QByteArray setEnableState();
    QByteArray setHalt();

    QByteArray setPorfileVelocityMode();
    QByteArray setMaximalProfileVelocity(quint16 value);
    QByteArray setQuickstopDeceleration(quint16 value);
    QByteArray setProfileAcceleration(quint16 value);
    QByteArray setProfileDeceleration(quint16 value);
    QByteArray setMaxAcceleration(quint16 value);

    QByteArray setTargetVelocity(quint16 velocity);
    qint32 getActualVelocity();
    quint32    ReadVelocity(QByteArray array);

    QByteArray setPositionControlWord();
    QByteArray setPorfilePositionMode();
    QByteArray setProfileVelocity(quint16 value);
    QByteArray setTargetPosition(qint32 value);
    QByteArray getActualPosition();
    qint32     ReadPosition(QByteArray array);

    QByteArray setHomeMode();
    QByteArray startHoming();
    QByteArray stopHoming();
    QByteArray setPositiveSpeed();
};

#endif
