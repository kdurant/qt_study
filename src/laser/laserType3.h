#ifndef LASERTYPE3_H
#define LASERTYPE3_H
#include "LaserController.h"
#include "bsp_config.h"
#include "protocol.h"

// 无人机雷达激光控制器, PQNL 40
class LaserType3 : public LaserController
{
    Q_OBJECT
private:
    bool       isRecvNewData;  // 是否收到数据
    QByteArray recvData;

    quint8 checksum(QVector<quint8>& data)
    {
        quint8 ret = 0;
        for(int i = 0; i < 8; i++)
            ret += data[i];
        return ret;
    }

public:
    LaserType3()
    {
        isRecvNewData = false;
    }

    struct LaserInfo
    {
        quint32 freq_outside;
        quint32 freq_inside;
        quint32 work_time;
    };

    bool setMode(OpenMode mode) override;

    bool open(void) override;

    bool close(void) override;

    bool setFreq(qint32 freq) override;

    bool setCurrent(quint16 current);

    bool getStatus(void);

signals:
    void laserInfoReady(LaserInfo& data);  // 接收到响应数据

public slots:
    void setNewData(QByteArray& data)
    {
        isRecvNewData = true;
        recvData      = data;
        emit responseDataReady();
    }
};

#endif
