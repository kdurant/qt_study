#ifndef LASERTYPE3_H
#define LASERTYPE3_H
// 陆地雷达激光控制器
#include "LaserController.h"
#include "bsp_config.h"
#include "protocol.h"

class LaserType3 : public LaserController
{
private:
    bool       isRecvNewData;  // 是否收到数据
    QByteArray recvData;

public:
    LaserType3()
    {
        isRecvNewData = false;
    }

    bool open(void) override;

    bool close(void) override;

    bool setFreq(qint32 freq);
    bool setCurrent(qint32 current);

public slots:

    void setNewData(QByteArray& data)
    {
        isRecvNewData = true;
        recvData      = data;
        emit responseDataReady();
    }
};

#endif
