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
    command[6] = (power >> 8) & 0xff;
    command[7] = power & 0xff;
    command[8] = checksum(command);
    QByteArray frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}

void LaserType4::getStatus(QByteArray &data)
{
    QByteArray frame;
    switch(data[2])
    {
        case 0x00:
            info.statusBit = data[32];
            info.errorBit  = data[33];
            info.headTemp  = data[34];
            break;
        case 0x0a:
            info.expected_current = ((static_cast<uint8_t>(data[4])) << 8) +
                                    ((static_cast<uint8_t>(data[5])) << 0);
            info.real_current = ((static_cast<uint8_t>(data[6])) << 8) +
                                ((static_cast<uint8_t>(data[7])) << 0);
            break;
        case 0x3c:
            frame       = data.mid(8, 4);
            info.ldTemp = Common::ba2int(frame, 1);
            break;
        case 0x3e:
            frame                 = data.mid(8, 4);
            info.laserCrystalTemp = Common::ba2int(frame, 1);
            break;
        case 0x3f:
            frame                 = data.mid(8, 4);
            info.multiCrystalTemp = Common::ba2int(frame, 1);
            break;
        default:
            break;
    }
}
