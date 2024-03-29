#include "laserType2.h"

bool LaserType2::setMode(LaserController::OpenMode mode)
{
    return true;
}

/**
 * @brief LaserType2::open
 * 1. 使用00 00 00 02指令，设置激光器工作频率
 * 2. 使用00 00 00 28指令，打开激光器
 * @return
 */
bool LaserType2::open()
{
    QByteArray frame = BspConfig::int2ba(0x01);
    emit       sendDataReady(MasterSet::LASER_ENABLE, 4, frame);
    QThread::msleep(1);
    return true;
}

/**
 * @brief 关闭激光器
 * 1. 使用acc2命令将电流设置到0
 * 2. 使用00 00 00 28指令，关闭激光器
 * @return
 */
bool LaserType2::close()
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
bool LaserType2::setFreq(qint32 freq)
{
    QByteArray frame = BspConfig::int2ba(freq);
    emit       sendDataReady(MasterSet::LASER_FREQ, 4, frame);
    QThread::msleep(1);
    return true;
}

/**
 * @brief 最大值5000mA
 * 设置的值是整数，但相应的数据会在后面添加小数点
 * 例如：设置5000，返回5000.00
 * @return
 */
bool LaserType2::setCurrent(quint16 current)
{
    QByteArray packet{"ACC 2 "};
    packet.append(QString::number(current));
    packet.append("\r\n");
    emit sendDataReady(MasterSet::LASER_PENETRATE, packet.length(), packet);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType2::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();
    if(isRecvNewData)
    {
        isRecvNewData = false;
//        if(recvData.contains(packet.mid(0, packet.length() - 2)))
            return true;
//        else
//            return false;
    }
    return false;
}

bool LaserType2::getStatus()
{
    QString s;
    s = getCurrent();
    if(s != "error")
        info.real_current = static_cast<int>(s.split(' ')[0].toFloat(nullptr));

    s = getFreq();
    if(s != "error")
        info.freq_outside = static_cast<int>(s.split(' ')[0].toInt(nullptr));

    s = getTemp();
    if(s != "error")
        info.temp = info.temp = s.split(' ')[0].toFloat(nullptr);

    s = getSwitch();
    if(s == "close")
        info.status = 0;
    else
        info.status = 1;

    emit laserInfoReady(info);
    return true;
}

QString LaserType2::getCurrent()
{
    QByteArray packet{"ACC 2"};
    packet.append("\r\n");
    emit sendDataReady(MasterSet::LASER_PENETRATE, packet.length(), packet);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType2::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();
    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(recvData.contains("LASER2"))
            return recvData.mid(8, recvData.length() - 2 - 8);
        else
            return "error";
    }
    return "error";
}

QString LaserType2::getFreq()
{
    QByteArray packet{"FM"};
    packet.append("\r\n");
    emit sendDataReady(MasterSet::LASER_PENETRATE, packet.length(), packet);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType2::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();
    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(recvData.contains("FREQUENCY"))
            return recvData.mid(11, recvData.length() - 2 - 11);
        else
            return "error";
    }
    return "error";
}

QString LaserType2::getTemp()
{
    QByteArray packet{"IT"};
    packet.append("\r\n");
    emit sendDataReady(MasterSet::LASER_PENETRATE, packet.length(), packet);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType2::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();
    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(recvData.contains("TINT"))
            return recvData.mid(6, recvData.length() - 2 - 6);
        else
            return "error";
    }
    return "error";
}

/**
 * @brief LaserType2::getStatus
 * @return
 */

QString LaserType2::getSwitch()
{
    QByteArray packet{"AM"};
    packet.append("\r\n");
    emit sendDataReady(MasterSet::LASER_PENETRATE, packet.length(), packet);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType2::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();
    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(recvData.contains("AM: O"))
            return "close";
        else if(recvData.contains("AM: C"))
            return "open";
    }
    return "exception";
}
