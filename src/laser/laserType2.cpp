#include "laserType2.h"

/**
 * @brief LaserType2::open
 * 1. 使用00 00 00 02指令，设置激光器工作频率
 * 2. 使用00 00 00 28指令，打开激光器
 * @return
 */
bool LaserType2::open()
{
}

/**
 * @brief 最大值5000mA
 * 设置的值是整数，但相应的数据会在后面添加小数点
 * 例如：设置5000，返回5000.00
 * @return
 */
bool LaserType2::setCurrent(qint32 current)
{
    QByteArray packet{"Acc 2 "};
    packet.append(QString::number(current));
    packet.append("\r\n");
    emit sendDataReady(packet);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType2::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();
    if(isRecvNewData)
    {
        if(recvData.contains(packet.mid(0, packet.length() - 2)))
            return true;
        else
            return false;
    }
    else
        return false;
}
