
#include "laserType3.h"
#include <numeric>

bool LaserType3::setMode(LaserController::OpenMode mode)
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x01, 0xee, 0x00, 0x00, 0x00, 0x00, 0xff, 0x33, 0xcc};
    command[4] = mode;
    command[9] = checksum(command);
    QByteArray frame;
    for (int i = 0; i < command.length(); i++) {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_ENABLE, frame.length(), frame);
    return true;
}

/**
 * @brief LaserType3::open
 * @return
 */
bool LaserType3::open()
{
    QByteArray frame = BspConfig::int2ba(0x01);
    emit       sendDataReady(MasterSet::LASER_ENABLE, 4, frame);
    QThread::msleep(1);
    return true;
}

/**
 * @brief 关闭激光器
 * @return
 */
bool LaserType3::close()
{
    bool status = setCurrent(0);
    if(!status)
    {
        return false;
    }
    QByteArray frame = BspConfig::int2ba(0x00);
    emit       sendDataReady(MasterSet::LASER_ENABLE, 4, frame);
    return true;
}

/**
 * @brief 使用00 00 00 02指令，设置激光器工作频率
 * @param freq
 * @return
 */
bool LaserType3::setFreq(qint32 freq)
{
    QByteArray frame = BspConfig::int2ba(freq);
    emit       sendDataReady(MasterSet::LASER_FREQ, 4, frame);
    QThread::msleep(1);
    return true;
}

/**
 * @brief 最大值电流，单位0.01A
 * @return
 */
bool LaserType3::setCurrent(quint16 current)
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x20, 0xee, 0x00, 0x00, 0x00, 0x00, 0xff, 0x33, 0xcc};
    command[4] = current & 0xff;
    command[5] = (current >> 8) & 0xff;
    command[9] = checksum(command);
    QByteArray frame;
    for (int i = 0; i < command.length(); i++) {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_ENABLE, frame.length(), frame);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType3::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();

    return false;
}
