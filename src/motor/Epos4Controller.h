#ifndef EPOS4_H
#define EPOS4_H
#include <QtCore>
#include "MotorController.h"

class EPOS4 : public MotorController
{
public:
    EPOS4() = default;

public:
    MOTOR_STATUS init(void) override;
    MOTOR_STATUS run(quint16 speed) override;

    qint32 getActualVelocity(void) override;
    qint32 getActualPosition(void) override;

    MOTOR_STATUS moveToPosition(double postion, int direct) override;
    bool         moveToHome(void) override;
    bool         moveFixSpeed(quint32 speed) override;

    bool sweep(quint32 start_pos, quint32 end_pos);

private:
    bool       isRecvNewData{false};  // 是否收到数据
    QByteArray recvData;
    qint32     waitTime{500};

    quint32 maxVelocity{3571};
    quint32 quickStopDeceleration{45};

    void waitResponse(quint16 waitMS)
    {
        QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
        connect(this, &EPOS4::responseDataReady, &waitLoop, &QEventLoop::quit);
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

private:
    QVector<uint8_t> frame;
    QVector<uint8_t> packetControlWordFrame(uint16_t index, uint8_t nodeId, uint8_t subindex, uint16_t default_value)
    {
        frame.clear();
        frame.append(0x90);
        frame.append(0x02);
        frame.append(0x68);
        frame.append(0x04);
        frame.append(nodeId);
        frame.append((index >> 8) & 0xff);
        frame.append((index >> 0) & 0xff);
        frame.append(subindex);
        frame.append((default_value >> 8) & 0xff);
        frame.append((default_value >> 0) & 0xff);
        frame.append(0x00);
        frame.append(0x00);
        return frame;
    };

    quint16          calcFieldCRC(quint16* pDataArray, quint16 numberofWords);  // CRC-CCITT
    QVector<uint8_t> WordPlusCRC(QVector<uint8_t>& word);
    QByteArray       transmitWord2Byte(const QVector<uint8_t>& word);

    bool clearFault();
    bool setShutdown();
    bool setEnableState();
    bool setHalt();
    bool setProfileVelocityMode();
    bool setMaxProfileVelocity(uint16_t value = 0x0df3);
    bool setProfileVelocity(quint16 value);
    bool setQuickstopDeceleration(quint16 value = 45);
    bool setProfileAcceleration(quint16 value = 45);
    bool setProfileDeceleration(quint16 value = 45);

    bool setHomeMode();
};

#endif
