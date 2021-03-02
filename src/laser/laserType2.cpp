#include "laserType2.h"

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
bool LaserType2::setCurrent(qint32 current)
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
        if(recvData.contains(packet.mid(0, packet.length() - 2)))
            return true;
        else
            return false;
    }
    return false;
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
