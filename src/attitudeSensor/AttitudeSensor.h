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
        data.x    = ((static_cast<quint8>(frame[3]) << 8) | static_cast<quint8>(frame[2])) / 32768.0 * 16 * 9.8;
        data.y    = ((static_cast<quint8>(frame[5]) << 8) | static_cast<quint8>(frame[4])) / 32768.0 * 16 * 9.8;
        data.z    = ((static_cast<quint8>(frame[7]) << 8) | static_cast<quint8>(frame[6])) / 32768.0 * 16 * 9.8;
        data.temp = ((static_cast<quint8>(frame[9]) << 8) | static_cast<quint8>(frame[8])) / 100.0;

        return data;
    }
    AttitudeInfo getAngularVelocity(QByteArray& frame)
    {
        AttitudeInfo data;
        data.x    = ((static_cast<quint8>(frame[3]) << 8) | static_cast<quint8>(frame[2])) / 32768.0 * 2000;
        data.y    = ((static_cast<quint8>(frame[5]) << 8) | static_cast<quint8>(frame[4])) / 32768.0 * 2000;
        data.z    = ((static_cast<quint8>(frame[7]) << 8) | static_cast<quint8>(frame[6])) / 32768.0 * 2000;
        data.temp = ((static_cast<quint8>(frame[9]) << 8) | static_cast<quint8>(frame[8])) / 100.0;

        return data;
    }
    AttitudeInfo getAngular(QByteArray& frame)
    {
        AttitudeInfo data;

        data.x    = ((static_cast<quint8>(frame[3]) << 8) | static_cast<quint8>(frame[2])) / 32768.0 * 180;
        data.y    = ((static_cast<quint8>(frame[5]) << 8) | static_cast<quint8>(frame[4])) / 32768.0 * 180;
        data.z    = ((static_cast<quint8>(frame[7]) << 8) | static_cast<quint8>(frame[6])) / 32768.0 * 180;
        data.temp = ((static_cast<quint8>(frame[9]) << 8) | static_cast<quint8>(frame[8])) / 100.0;

        return data;
    }
    AttitudeInfo getMagneticField(QByteArray& frame)
    {
        AttitudeInfo data;
        data.x    = ((static_cast<quint8>(frame[3]) << 8) | static_cast<quint8>(frame[2])) / 32768.0 * 180;
        data.y    = ((static_cast<quint8>(frame[5]) << 8) | static_cast<quint8>(frame[4])) / 32768.0 * 180;
        data.z    = ((static_cast<quint8>(frame[7]) << 8) | static_cast<quint8>(frame[6])) / 32768.0 * 180;
        data.temp = ((static_cast<quint8>(frame[9]) << 8) | static_cast<quint8>(frame[8])) / 100.0;

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
