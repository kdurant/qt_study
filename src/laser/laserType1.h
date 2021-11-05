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
        isRecvNewData = false;
    }

    bool setMode(OpenMode mode) override;

    bool open(void) override;

    bool close(void) override;
    bool setPower(quint16 power) override;

    bool setLD2DlyTime(quint16 value);

    bool getStatus(void);

public slots:
    void setNewData(QByteArray& data)
    {
        recvData = data;
        emit responseDataReady();
    }
};

#endif
