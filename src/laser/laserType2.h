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
        isRecvNewData = false;
    }

    bool open(void) override;

    bool close(void) override;

    bool    setFreq(qint32 freq) override;
    bool    setCurrent(qint32 current);
    QString getCurrent(void);
    QString getFreq(void);
    QString getTemp(void);
    QString getStatus(void);

public slots:
    void setNewData(QByteArray& data)
    {
        isRecvNewData = true;
        recvData      = data;
        emit responseDataReady();
    }
};

#endif
