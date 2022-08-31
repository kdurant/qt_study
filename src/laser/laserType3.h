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
        isRecvNewData         = false;
        info.status           = -1;
        info.work_time        = -1;
        info.freq_outside     = -1;
        info.freq_inside      = -1;
        info.expected_current = -1;
        info.real_current     = -1;
        info.temp             = -1;
        info.headTemp         = -1;
        info.ldTemp           = -1;
        info.laserCrystalTemp = -1;
        info.multiCrystalTemp = -1;
        info.statusBit        = 0;
        info.errorBit         = 0;
    }

    bool setMode(OpenMode mode) override;
    bool open(void) override;
    bool close(void) override;
    bool setFreq(qint32 freq) override;
    bool setCurrent(quint16 current) override;
    bool getStatus(void) override;

public slots:
    void setNewData(const QByteArray& data) override
    {
        isRecvNewData = true;
        recvData      = data;
        emit responseDataReady();
    }
};

#endif
