#ifndef LASERTYPE3_H
#define LASERTYPE3_H
// 无人机雷达激光控制器, PQNL 40
#include "LaserController.h"
#include "bsp_config.h"
#include "protocol.h"

class LaserType3 : public LaserController
{
private:
    bool       isRecvNewData;  // 是否收到数据
    QByteArray recvData;

    quint8 checksum(QVector<quint8> &data)
    {
        quint8 ret = 0;
        for (int i = 0; i < 8; i++)
            ret += data[i];
        return ret;
    }

public:
    LaserType3()
    {
        isRecvNewData = false;
    }

    bool setMode(OpenMode mode) override;

    bool open(void) override;

    bool close(void) override;

    bool setFreq(qint32 freq) override;

    bool setCurrent(quint16 current);

    bool getStatus(void);

public slots:

    void setNewData(QByteArray& data)
    {
        isRecvNewData = true;
        recvData      = data;
        emit responseDataReady();
    }
};

#endif
