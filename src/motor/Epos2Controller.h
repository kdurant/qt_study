#ifndef EPOS2_H
#define EPOS2_H
#include <QtCore>
#include "MotorController.h"

class EPOS2 : public MontorController
{
public:
    EPOS2() = default;

public:
    bool init(void) override;
    bool start(void) override;
    bool stop(void) override;
    bool run(quint16 speed) override;

    bool moveToPosition(quint32 postion);
    bool moveFixSpeed(quint32 speed);
    bool moveToHome(void);

private:
    bool       isRecvNewData{false};  // 是否收到数据
    QByteArray recvData;
    qint32     waitTime{1000};

    quint32 maxVelocity{3571};
    quint32 quickStopDeceleration{45};

    void waitResponse(quint16 waitMS)
    {
        QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
        connect(this, &EPOS2::responseDataReady, &waitLoop, &QEventLoop::quit);
        QTimer::singleShot(waitMS, &waitLoop, &QEventLoop::quit);
        waitLoop.exec();
    }

    bool sendFrameStep1(QByteArray frame);
    bool sendFrameStep2(void);

    bool isResponse4f(void)
    {
        if(isRecvNewData)
        {
            isRecvNewData = false;
            return recvData.length() == 1 && recvData.at(0) == 0x4f;
        }
        return false;
    }
    bool isResponse4f00(void)
    {
        if(isRecvNewData)
        {
            isRecvNewData = false;
            return recvData.length() == 2 && recvData.at(0) == 0x4f && recvData.at(1) == 0x00;
        }
        return false;
    }

    bool send_4f_actively(void)
    {
        QByteArray frame(1, 0x4f);
        emit       sendDataReady(MasterSet::MOTOR_PENETRATE, 1, frame);
        return true;
    }

public slots:

    void setNewData(QByteArray& data)
    {
        isRecvNewData = true;
        recvData      = data;
        emit responseDataReady();
    }

public:
    quint16          calcFieldCRC(quint16* pDataArray, quint16 numberofWords);  //CRC-CCITT
    QVector<quint16> WordPlusCRC(QVector<quint16> word);
    QByteArray       transmitWord2Byte(QVector<quint16> word);
    QVector<quint16> receiveByte2Word(QByteArray array);

    bool setDisableState();
    bool clearFault();
    bool setShutdown();
    bool setEnableState();
    bool setHalt();

    bool       setProfileVelocityMode();
    bool       setMaximalProfileVelocity(quint16 value);
    bool       setQuickstopDeceleration(quint16 value);
    bool       setProfileAcceleration(quint16 value);
    bool       setProfileDeceleration(quint16 value);
    QByteArray setMaxAcceleration(quint16 value);

    bool   setTargetVelocity(quint16 velocity);
    qint32 getActualVelocity();

    QByteArray setPositionControlWord();
    bool       setProfilePositionMode();
    bool       setProfileVelocity(quint16 value);
    bool       setTargetPosition(quint32 value);
    qint32     getActualPosition();

    bool setAbsolutePositionStartImmdeitaly(void);

    bool setHomeMode();
    bool startHoming();
    bool stopHoming();
    bool setPositiveSpeed();
};

#endif
