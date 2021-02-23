#ifndef LASERCONTROLLER_H
#define LASERCONTROLLER_H
#include <QtCore>

class LaserController : public QObject
{
    Q_OBJECT

public:
    enum OpenMode
    {
        IN_SIDE = 0x01,
        OUT_SIDE
    };

    LaserController() = default;

    // API函数准备好控制数据帧
    virtual bool setMode(OpenMode mode) const { return false; };
    virtual bool setPower(qint8 power) const { return false; };
    virtual bool open(void) const { return false; };
    virtual bool close(void) const { return false; };

signals:
    void sendDataReady(QByteArray& data);
};
#endif
