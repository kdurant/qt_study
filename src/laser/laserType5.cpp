
#include "laserType5.h"
#include <numeric>

bool LaserType5::setMode(LaserController::OpenMode mode)
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x01, 0xee, 0x00, 0x00, 0x00, 0xff, 0x33, 0xcc};
    command[4] = mode;
    command[8] = checksum(command);
    QByteArray frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}

bool LaserType5::open()
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x33, 0xcc};
    QByteArray      frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}

bool LaserType5::close()
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x33, 0xcc};
    QByteArray      frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}

bool LaserType5::setFreq(qint32 freq)
{
    QByteArray frame = BspConfig::int2ba(freq);
    emit       sendDataReady(MasterSet::LASER_FREQ, 4, frame);
    QThread::msleep(1);
    return true;
}

//震荡级电流设置指令, green(532)
bool LaserType5::setCurrent(quint16 current)
{
    QVector<quint8> command{0x55, 0xAA, 0x0A, 0x01, 0xee, 0xee, 0x00, 0x00, 0xff, 0x33, 0xcc};
    command[4] = current & 0xff;
    command[5] = (current >> 8) & 0xff;
    command[8] = checksum(command);
    QByteArray frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    Common::sleepWithoutBlock(250);

    return true;
}

//放大级电流设置指令, blue(486)
// 上位机设置为70A时，下传参数值为7000
bool LaserType5::setPower(quint16 power)
{
    QVector<quint8> command{0x55, 0xAA, 0x0B, 0x01, 0xee, 0xee, 0x00, 0x00, 0xff, 0x33, 0xcc};
    command[4] = power & 0xff;
    command[5] = (power >> 8) & 0xff;
    command[8] = checksum(command);
    QByteArray frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    Common::sleepWithoutBlock(250);

    return true;
}

/**
 * @brief 每发送一次查询命令，激光器只会返回一种状态数据（总共有5种)
 * @return
 */
bool LaserType5::getStatus()
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x33, 0xcc};
    QByteArray      frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType5::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();

    if(recvData.size() == 0x32)
    {
        switch(recvData[2])
        {
            case 0x00:
                info.freq_outside = static_cast<quint8>(recvData[4]) +
                                    (static_cast<quint8>(recvData[5]) << 8) +
                                    (static_cast<quint8>(recvData[6]) << 16) +
                                    (static_cast<quint8>(recvData[7]) << 24);

                info.freq_inside = static_cast<quint8>(recvData[8]) +
                                   (static_cast<quint8>(recvData[9]) << 8) +
                                   (static_cast<quint8>(recvData[10]) << 16) +
                                   (static_cast<quint8>(recvData[11]) << 24);

                info.work_time = static_cast<quint8>(recvData[36]) +
                                 (static_cast<quint8>(recvData[37]) << 8) +
                                 (static_cast<quint8>(recvData[38]) << 16) +
                                 (static_cast<quint8>(recvData[39]) << 24);

                info.statusBit = static_cast<quint8>(recvData[45]);
                info.errorBit  = static_cast<quint8>(recvData[46]);
                emit laserInfoReady(info);
                return true;

                break;
            case 0x0a:
                info.expected_current = ((static_cast<uint8_t>(recvData[4])) << 0) +
                                        ((static_cast<uint8_t>(recvData[5])) << 8);
                info.real_current = ((static_cast<uint8_t>(recvData[6])) << 0) +
                                    ((static_cast<uint8_t>(recvData[7])) << 8);
                break;
            default:
                break;
        }
    }

    return false;
}

bool LaserType5::checkself()
{
    return true;
}

bool LaserType5::reset()
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x33, 0xcc};
    QByteArray      frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}
