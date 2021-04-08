#ifndef ATTITUDESENSOR_H
#define ATTITUDESENSOR_H
#include <QtCore>

class AttitudeSensor : public QObject
{
    Q_OBJECT
public:
    AttitudeSensor() = default;
    struct AttitudeInfo
    {
        double x;
        double y;
        double z;
        double temp;
    };

    AttitudeInfo accelerate;
    AttitudeInfo angularVelocity;
    AttitudeInfo angular;
    AttitudeInfo magneticField;

private:
    AttitudeInfo getAccelerate(QByteArray& frame)
    {
        AttitudeInfo data;
        data.x    = ((frame[2] << 8) | frame[3]) / 32768 * 16 * 9.8;
        data.y    = ((frame[4] << 8) | frame[5]) / 32768 * 16 * 9.8;
        data.z    = ((frame[6] << 8) | frame[7]) / 32768 * 16 * 9.8;
        data.temp = ((frame[8] << 8) | frame[8]) / 100;

        return data;
    }
    AttitudeInfo getAngularVelocity(QByteArray& frame)
    {
        AttitudeInfo data;
        data.x    = ((frame[2] << 8) | frame[3]) / 32768 * 2000;
        data.y    = ((frame[4] << 8) | frame[5]) / 32768 * 2000;
        data.z    = ((frame[6] << 8) | frame[7]) / 32768 * 2000;
        data.temp = ((frame[8] << 8) | frame[8]) / 100;

        return data;
    }
    AttitudeInfo getAngular(QByteArray& frame)
    {
        AttitudeInfo data;
        data.x    = ((frame[2] << 8) | frame[3]) / 32768 * 180;
        data.y    = ((frame[4] << 8) | frame[5]) / 32768 * 180;
        data.z    = ((frame[6] << 8) | frame[7]) / 32768 * 180;
        data.temp = ((frame[8] << 8) | frame[8]) / 100;

        return data;
    }
    AttitudeInfo getMagneticField(QByteArray& frame)
    {
        AttitudeInfo data;
        data.x    = ((frame[2] << 8) | frame[3]) / 32768 * 180;
        data.y    = ((frame[4] << 8) | frame[5]) / 32768 * 180;
        data.z    = ((frame[6] << 8) | frame[7]) / 32768 * 180;
        data.temp = ((frame[8] << 8) | frame[8]) / 100;

        return data;
    }

signals:
    void
    sendAttitudeResult(AttitudeInfo accelerate,
                       AttitudeInfo angularVelocity,
                       AttitudeInfo angular,
                       AttitudeInfo magneticField);
public slots:
    void parserFrame(QByteArray& data)
    {
        if(data.size() != 0x2c)
            return;
        QByteArray frame;

        frame      = data.mid(0, 11);
        accelerate = getAccelerate(frame);

        frame           = data.mid(11, 11);
        angularVelocity = getAngular(frame);

        frame   = data.mid(22, 11);
        angular = getAccelerate(frame);

        frame         = data.mid(33, 11);
        magneticField = getMagneticField(frame);

        emit sendAttitudeResult(accelerate, angularVelocity, angular, magneticField);
    }
};
#endif
