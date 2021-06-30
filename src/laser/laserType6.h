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
private:
    bool       isRecvNewData;  // 是否收到数据
    QByteArray recvData;

public:
    LaserType6()
    {
        isRecvNewData = false;
    }
    struct LaserInfo
    {
        int temp;
        int status;  // on or off
        int error;
        int current;
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

    bool checkError(void);
    int  getPower(void);
    int  getTemp(void);
    bool setHighVoltStatus(int value);
    bool setJitterFree(void);

    int  getError(void);
    bool clearError(void);
    int  getWorkStatus(void);
    int  getLDcurrent(void);

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
