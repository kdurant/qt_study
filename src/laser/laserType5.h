#ifndef LASERTYPE5_H
#define LASERTYPE5_H
#include "LaserController.h"
#include "bsp_config.h"
#include "protocol.h"

/**
 * @brief  双波长雷达激光控制器
 1. 波特率115200、8bit、无校验位
 2. 开光指令：55 AA 00 0C 00 00 00 00 YY 33 CC(YY是前8个字节的累加和、取低八位)
 3. 关光指令：55 AA 00 0D 00 00 00 00 YY 33 CC(YY是前8个字节的累加和、取低八位)
 4. 触发模式选择指令：55 AA 00 01 MM 00 00 00 YY 33 CC(YY是前8个字节的累加和、取低八位,MM是00时为内触发、MM是01时为外触发)
 5.  状态查询指令：55 AA 00 FF 00 00 00 00 YY 33 CC(YY是前8个字节的累加和、取低八位)
		例：55 AA 00 FF 00 00 00 00 FE 33 CC。
		备注：上位机周期性（例如100ms依次）下发状态查询指令时，激光器会依次轮流上传主控板状态数据包、振荡级驱动板状态数据包、放大级驱动板状态数据包、温控BBO1状态数据包、温控BBO2状态数据包、温控LBO1状态数据包、温控LBO2状态数据包。各个状态数据包的详细内容，见后文表格。
 6.  放大级电流设置指令：55 AA 0B 01 MM NN 00 00 YY 33 CC(YY是前8个字节的累加和、取低八位,MM为电流设定值的低八位、NN为电流设定值的高八位。单位0.01A)
		例：55 AA 0B 01 E8 03 00 00 F6 33 CC,即设置放大级电流10.00A
 */
class LaserType5 : public LaserController
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
    LaserType5()
    {
        isRecvNewData = false;
    }

    struct LaserInfo
    {
        quint32 freq_outside;
        quint32 freq_inside;
        quint32 work_time;
        quint8  status_bit;
        quint8  error_bit;
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
