#ifndef LASERTYPE6_H
#define LASERTYPE6_H
#include "protocol.h"
#include "ProtocolDispatch.h"
#include "LaserController.h"
#include "common.h"

// 海二所雷达激光控制器
// Nanosecond Nd-YAG
class LaserType6 : public LaserController
{
    Q_OBJECT

public:
    LaserType6()
    {
        isRecvNewData = false;
    }
    struct LaserInfo
    {
        QString temp;
        QString status;  // on or off
        QString error;
        QString current;
    };

    bool setMode(OpenMode mode) override;

    bool open(void) override;

    bool close(void) override;
    bool setFreq(qint32 freq) override;
    bool setPower(quint16 power) override;
    bool getInfo(void);

    QString getValueField(QByteArray s)
    {
        int start = s.indexOf(' ') + 1;
        int end   = s.indexOf('\r');
        return s.mid(start, end - start);
    }

    bool    checkError(void);
    QString getPower(void);
    QString getTemp(void);
    bool    setHighVoltStatus(int value);
    bool    setJitterFree(void);

    QString getError(void);
    bool    clearError(void);
    QString getWorkStatus(void);

signals:
    void laserInfoReady(LaserInfo& data);  // 接收到响应数据

public slots:
    void setNewData(QByteArray& data)
    {
        recvData = data;
        emit responseDataReady();
    }
};

#endif
