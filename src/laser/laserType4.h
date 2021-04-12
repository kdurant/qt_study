#ifndef LASERTYPE4_H
#define LASERTYPE4_H
#include "protocol.h"
#include "ProtocolDispatch.h"
#include "LaserController.h"

// 水下雷达雷达激光控制器
// 5kHz/532nm/500uj 微脉冲激光器
/*
说明:
格式为 16 进制字节。
X1,X2,X3,X4 代表数据位。 X5 代表校验和（取低 8 位） 。
指令:
1、 一键开光 ： 55 AA 00 0B 00 00 00 01 0B 33 CC
（在激光器上电 60s 之后发送命令才可以正确响应开光功能。 ）
2、 一键关光 ： 55 AA 00 0C 00 00 00 01 0C 33 CC
3、 设置主控触发_外触发 ： 55 AA 00 01 00 00 00 01 01 33 CC
4、 设置主控触发_内触发 ： 55 AA 00 01 00 00 00 00 00 33 CC
5、 错误复位 ： 55 AA 00 0D 00 00 00 00 0C 33 CC
6、 设置 LD 电流(0-3.20A) ： 55 AA 0A 01 00 00 X3 X4 X5 33 CC
（单位： 0.01A。例如 300 代表 3A。设定电流为 3A， 则 X3:01,X4:2C,X5:37(前
边八个字节的和取低八位)， 指令为： 55 AA 0A 01 00 00 01 2C 37 33 CC
（数据位高位在前））
*/
class LaserType4 : public LaserController
{
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
    LaserType4()
    {
        isRecvNewData = false;
    }
    struct LaserInfo
    {
        quint8 statusBit;
        quint8 errorBit;
    };

    bool setMode(OpenMode mode) override;

    bool open(void) override;

    bool close(void) override;
    bool setPower(quint16 power) override;

signals:
    void laserInfoReady(BspConfig::Gps_Info& data);  // 接收到响应数据

public slots:
    void setNewData(QByteArray& data)
    {
        recvData = data;
        emit responseDataReady();
    }
};

#endif
