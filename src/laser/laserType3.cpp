
#include "laserType3.h"
#include <numeric>

/**
 * @brief LaserType3::setMode
 * 设置内触发 0x55, 0xAA, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0xcc、
 * 设置外触发 0x55, 0xAA, 0x00, 0x01, 0xe1, 0x00, 0x00, 0x00, 0x01, 0x33, 0xcc
 * 除了获得状态命令之外，其他命令均无响应
 * @param mode
 * @return
 */
bool LaserType3::setMode(LaserController::OpenMode mode)
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

/**
 * @brief LaserType3::open
 * 激光器开光 0x55, 0xAA, 0x00, 0x0C, 0x04, 0x00, 0x00, 0x00, 0x0f, 0x33, 0xcc
 * @return
 */
bool LaserType3::open()
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x0C, 0x04, 0x00, 0x00, 0x00, 0x0f, 0x33, 0xcc};
    QByteArray      frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}

/**
 * @brief 关闭激光器
 * 激光器关光 0x55, 0xAA, 0x00, 0x0D, 0x04, 0x00, 0x00, 0x00, 0x10, 0x33, 0xcc
 * @return
 */
bool LaserType3::close()
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0x0D, 0x04, 0x00, 0x00, 0x00, 0x10, 0x33, 0xcc};
    QByteArray      frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
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
    QVector<quint8> command{0x55, 0xAA, 0x0A, 0x01, 0xee, 0x00, 0x00, 0x00, 0xff, 0x33, 0xcc};
    command[4] = current & 0xff;
    command[5] = (current >> 8) & 0xff;
    command[8] = checksum(command);
    QByteArray frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_ENABLE, frame.length(), frame);

    //    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    //    connect(this, &LaserType3::responseDataReady, &waitLoop, &QEventLoop::quit);
    //    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    //    waitLoop.exec();

    return true;
}

bool LaserType3::getStatus()
{
    QVector<quint8> command{0x55, 0xAA, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x33, 0xcc};
    QByteArray      frame;
    for(int i = 0; i < command.length(); i++)
    {
        frame.append(command[i] & 0xff);
    }
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType3::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();

    return true;
}
