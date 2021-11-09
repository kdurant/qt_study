#ifndef LASERTYPE1_H
#define LASERTYPE1_H
#include "protocol.h"
#include "ProtocolDispatch.h"
#include "LaserController.h"

// 海洋雷达激光控制器
//    1	内触发选择（关光状态下选择）	55 AA 01 01 00 01 33 CC	  55 AA 01 01 00 01 33 CC
//    2	外触发选择（关光状态下选择）	55 AA 01 01 00 02 33 CC	  55 AA 01 01 00 02 33 CC
//    3	出光	  55 AA 01 02 00 01 33 CC	55 AA 01 02 00 01 33 CC
//    4	关光	  55 AA 01 02 00 02 33 CC	55 AA 01 02 00 02 33 CC
//    5	LD2延时设置	55 AA 01 07 XX XX 33 CC	  55 AA 01 07 XX XX 33 CC
//    6	TEC1温度设置	55 AA 02 01 XX XX 33 CC	  55 AA 02 01 XX XX 33 CC
//    7	TEC2温度设置	55 AA 02 02 XX XX 33 CC   55 AA 02 02 XX XX 33 CC
class LaserType1 : public LaserController
{
    Q_OBJECT

public:
    LaserType1()
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
    bool setPower(quint16 power) override;

    bool getStatus(void) override;

private:
    bool setLD2DlyTime(quint16 value);

public slots:
    void setNewData(QByteArray& data) override
    {
        recvData = data;
        emit responseDataReady();
    }
};

#endif
