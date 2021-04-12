#include "laserType4.h"

bool LaserType4::setMode(LaserController::OpenMode mode)
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0xcc};
    command[7] = mode;
    command[8] = mode;
    QByteArray frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);

    return true;
}

bool LaserType4::open()
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x01, 0x0B, 0x33, 0xcc};
    QByteArray      frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}

bool LaserType4::close()
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x0C, 0x33, 0xcc};
    QByteArray      frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}

bool LaserType4::setPower(quint16 power)
{
    QVector<quint8> command{0x55, 0xAA, 0x0A, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0xcc};
    command[6] = power & 0xff;
    command[7] = (power >> 8) & 0xff;
    command[8] = checksum(command);
    QByteArray frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_ENABLE, frame.length(), frame);
    return true;
}
