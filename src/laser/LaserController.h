#ifndef LASERCONTROLLER_H
#define LASERCONTROLLER_H
#include <QtCore>

class LaserController : public QObject
{
    Q_OBJECT

public:
    enum OpenMode
    {
        IN_SIDE  = 0x00,
        OUT_SIDE = 0x01,
    };

    LaserController() = default;

    // API函数准备好控制数据帧
    virtual bool setMode(OpenMode mode)
    {
        return false;
    };

    // 部分激光器以功率的形式
    // 部分激光器以电流的形式
    virtual bool setPower(quint16 power)
    {
        return false;
    };

    // 外触发时的工作频率
    virtual bool setFreq(qint32 freq)
    {
        return false;
    };

    virtual bool open(void)
    {
        return false;
    };
    virtual bool close(void)
    {
        return false;
    };

signals:
    void sendDataReady(qint32 command, qint32 data_len, QByteArray& data);  // 需要发送的数据已经准备好
    void responseDataReady(void);                                           // 接收到响应数据
};
#endif
