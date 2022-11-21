#ifndef LASERTYPE2_H
#define LASERTYPE2_H
#include "LaserController.h"
#include "bsp_config.h"
#include "protocol.h"

// 陆地雷达激光控制器
class LaserType2 : public LaserController
{
public:
    LaserType2()
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

private:
    QString getCurrent(void);
    QString getFreq(void);
    QString getTemp(void);
    QString getSwitch(void);

public slots:
    void setNewData(const QByteArray data) override
    {
        isRecvNewData = true;
        recvData      = data;
        emit responseDataReady();
    }
};

#endif
