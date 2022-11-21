#include "ProtocolDispatch.h"
#include <QMessageBox>

quint32 ProtocolDispatch::cmdNum = 0;

/**
 * @brief 根据协议预定好的命令，将收到的信息，发送给指定的模块处理
 * @param data
 */
void ProtocolDispatch::parserFrame(QByteArray data)
{
    //    qDebug() << "ProtocolDispatch::parserFrame thread id = " << QThread::currentThreadId();

    if(data.size() != 284)
        return;

    uint32_t   command  = getCommand(data);
    uint32_t   data_len = getDataLen(data);
    QByteArray transmitFrame;
    switch(command)
    {
        case SlaveUp::SYS_INFO:
            emit infoDataReady(data.mid(24, 256));
            break;
        case SlaveUp::COMMAND_CNT:
            break;
        case SlaveUp::PREVIEW_DATA:
            emit onlineDataReady(data);
            break;
        case SlaveUp::GPS_PENETRATE:
            emit gpsFrameReady(data.mid(24, data_len));
            break;
        case SlaveUp::LASER_PENETRATE:
            emit laserDataReady(data.mid(24, data_len));
            break;
        case SlaveUp::MOTOR_PENETRATE:
            transmitFrame = data.mid(24, data_len);
            emit motorDataReady(transmitFrame);
            break;
        case SlaveUp::ATTITUDE_PENETRATE:
            transmitFrame = data.mid(24, data_len);
            emit attitudeDataReady(transmitFrame);
            break;
        case SlaveUp::MS5837_PENETRATE:
            transmitFrame = data.mid(24, data_len);
            emit ms5837DataReady(transmitFrame);
            break;

        case SlaveUp::FLASH_DATA:
            emit flashDataReady(data);
            break;

        case SlaveUp::RESPONSE_SSD_UNIT:
            transmitFrame = data.mid(24, data_len);
            emit ssdDataReady(transmitFrame);
            break;
        case SlaveUp::AD_RETURN_DATA:
            emit ADDataReady(data);
            break;
        case SlaveUp::DA_RETURN_DATA:
            emit DADataReady(data);
            break;
        default:
            QString error = "Undefined command received!";
            emit    errorDataReady(error);
            break;
    }
}

void ProtocolDispatch::encode(qint32 command, qint32 data_len, QByteArray data)
{
    QByteArray frame;
    uint32_t   checksum = 0xeeeeffff;
    frame.append(QByteArray::fromHex("AA555AA5AA555AA5"));  // 帧头
    frame.append(
        QByteArray::fromHex(QByteArray::number(cmdNum++, 16).rightJustified(8, '0')));  // 指令序号
    frame.append(
        QByteArray::fromHex(QByteArray::number(command, 16).rightJustified(8, '0')));  // 命令
    frame.append(QByteArray::fromHex(
        QByteArray::number(packetNum, 16).rightJustified(8, '0')));  // 包序号，一般为0
    frame.append(QByteArray::fromHex(
        QByteArray::number(data_len, 16).rightJustified(8, '0')));  // 有效数据长度
    frame.append(data);                                             // 数据，总是256
    if(data_len <= 256)
        frame.append(256 - data_len, 0);
    else
        QMessageBox::warning(nullptr, "警告", "需要打包的数据过长");
    frame.append(
        QByteArray::fromHex(QByteArray::number(checksum, 16).rightJustified(8, '0')));  // 校验
    emit frameDataReady(frame);
}
