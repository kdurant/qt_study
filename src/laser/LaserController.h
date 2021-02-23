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

    virtual bool setMode(OpenMode mode) const;
    virtual bool setPower(qint8 power) const;
    virtual bool open(void) const;
    virtual bool close(void) const;

signals:
    void sendDataReady(QByteArray& data);
};
#endif
