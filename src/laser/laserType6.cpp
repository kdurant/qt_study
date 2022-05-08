#include "laserType6.h"

bool LaserType6::setMode(LaserController::OpenMode mode)
{
    QString s = "p102 ";
    if(mode == IN_SIDE)
        s.append("0");
    else
        s.append("1");
    s.append("\r\n");
    QByteArray frame = s.toUtf8();
    emit       sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}

bool LaserType6::open()
{
    QString    s     = "p1 1\r\n";
    QByteArray frame = s.toUtf8();
    emit       sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);

    Common::sleepWithoutBlock(15000);

    s     = "p15 1\r\n";
    frame = s.toUtf8();
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}

bool LaserType6::close()
{
    QString    s     = "p1 0\r\n";
    QByteArray frame = s.toUtf8();
    emit       sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);

    s     = "p15 0\r\n";
    frame = s.toUtf8();
    emit sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}

bool LaserType6::setFreq(qint32 freq)
{
    QString s = "p7 ";
    s.append(QString::number(freq, 10));
    s.append("\r\n");
    QByteArray frame = s.toUtf8();
    emit       sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);

    return true;
}

/**
 * @brief 单位：Amps
 *
 * @param power
 *
 * @return
 */
bool LaserType6::setPower(quint16 power)
{
    QString s = "p3 ";
    s.append(QString::number(power, 10));
    s.append("\r\n");
    QByteArray frame = s.toUtf8();
    emit       sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);

    return true;
}

bool LaserType6::getStatus()
{
    return true;
}

// bool LaserType6::getInfo(void)
//{
//     LaserInfo info{0, 0, 0, 0};
//     info.temp    = getTemp();
//     info.status  = getWorkStatus();
//     info.error   = getError();
//     info.current = getPower();

//    emit laserInfoReady(info);
//    return true;
//}

/**
 * @brief check if there is an error
 *
 * @return
 */
bool LaserType6::checkError(void)
{
    QString frame = "g10\r\n";
}

QString LaserType6::getPower(void)
{
    QString    s     = "g3\r\n";
    QByteArray frame = s.toUtf8();
    emit       sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType6::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();

    QString v = getValueField(recvData);
    return v;
}

QString LaserType6::getTemp(void)
{
    QString    s     = "g322\r\n";
    QByteArray frame = s.toUtf8();
    emit       sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType6::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();

    QString v = getValueField(recvData);
    return v;
}

bool LaserType6::setHighVoltStatus(int value)
{
    QString s = "p15 ";
    s.append(QString::number(value, 10));
    s.append("\r\n");
    QByteArray frame = s.toUtf8();
    emit       sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}

bool LaserType6::setJitterFree()
{
    QString    s     = "p122 21\r\n";
    QByteArray frame = s.toUtf8();
    emit       sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}

QString LaserType6::getError()
{
    QString    s     = "g10\r\n";
    QByteArray frame = s.toUtf8();
    emit       sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType6::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();

    QString v = getValueField(recvData);
    return v;
}

bool LaserType6::clearError()
{
    QString    s     = "p10 0\r\n";
    QByteArray frame = s.toUtf8();
    emit       sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);
    return true;
}

QString LaserType6::getWorkStatus()
{
    QString    s     = "g1\r\n";
    QByteArray frame = s.toUtf8();
    emit       sendDataReady(MasterSet::LASER_PENETRATE, frame.length(), frame);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType6::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();

    QString v = getValueField(recvData);
    return v;
}
