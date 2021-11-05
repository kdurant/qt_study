#include "laserType4.h"

bool LaserType4::setMode(LaserController::OpenMode mode)
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0xcc};
    command[7] = mode;
    command[8] = mode;
    QByteArray tmp;
    for(int i = 0; i < command.length(); i++)
    {
        tmp.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, tmp.length(), tmp);

    return true;
}

bool LaserType4::open()
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x01, 0x0B, 0x33, 0xcc};
    QByteArray      tmp;
    for(int i = 0; i < command.length(); i++)
    {
        tmp.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, tmp.length(), tmp);
    return true;
}

bool LaserType4::close()
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x0C, 0x33, 0xcc};
    QByteArray      tmp;
    for(int i = 0; i < command.length(); i++)
    {
        tmp.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, tmp.length(), tmp);
    return true;
}

bool LaserType4::setPower(quint16 power)
{
    QVector<quint8> command{0x55, 0xAA, 0x0A, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0xcc};
    command[6] = (power >> 8) & 0xff;
    command[7] = power & 0xff;
    command[8] = checksum(command);
    QByteArray tmp;
    for(int i = 0; i < command.length(); i++)
    {
        tmp.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, tmp.length(), tmp);
    return true;
}

bool LaserType4::getStatus(void)
{
    QByteArray tmp;
    switch(frame[2])
    {
        case 0x00:
            info.statusBit = frame[32];
            info.errorBit  = frame[33];
            info.headTemp  = frame[34];
            break;
        case 0x0a:
            info.expected_current = ((static_cast<uint8_t>(frame[4])) << 8) +
                                    ((static_cast<uint8_t>(frame[5])) << 0);
            info.real_current = ((static_cast<uint8_t>(frame[6])) << 8) +
                                ((static_cast<uint8_t>(frame[7])) << 0);
            break;
        case 0x3c:
            tmp         = frame.mid(8, 4);
            info.ldTemp = Common::ba2int(tmp, 1);
            break;
        case 0x3e:
            tmp                   = frame.mid(8, 4);
            info.laserCrystalTemp = Common::ba2int(tmp, 1);
            break;
        case 0x3f:
            tmp                   = frame.mid(8, 4);
            info.multiCrystalTemp = Common::ba2int(tmp, 1);
            break;
        default:
            break;
    }
    return true;
}
