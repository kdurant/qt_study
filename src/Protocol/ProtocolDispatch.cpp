#include "ProtocolDispatch.h"
#include <QMessageBox>

quint32 ProtocolDispatch::cmdNum = 0;

/**
 * @brief 根据协议预定好的命令，将收到的信息，发送给指定的模块处理
 * @param data
 */
void ProtocolDispatch::parserFrame(QByteArray &data)
{
    uint32_t command = getCommand(data);
    switch(command)
    {
        case SlaveUp::SYS_INFO:
            deviceVersion = data.mid(272, 8);
            emit infoDataReady(command, data);
            break;
        case SlaveUp::COMMAND_CNT:
            break;
        case SlaveUp::PREVIEW_DATA:
            //        case MasterSet::PREVIEW_ENABLE:
            //        case MasterSet::PREVIEW_RATIO:
            //        case MasterSet::FIRST_LEN:
            //        case MasterSet::FIRST_POS:
            //        case MasterSet::SAMPLE_LEN:
            //        case MasterSet::SECOND_LEN:
            //        case MasterSet::COMPRESS_LEN:
            //        case MasterSet::COMPRESS_RATIO:
            emit previewDataReady(data);
            break;
        case SlaveUp::LASER_PENETRATE:
            emit laserDataReady(data);
            break;
        case SlaveUp::MOTOR_PENETRATE:
            emit motorDataReady(data);
            break;
        case SlaveUp::FLASH_DATA:
            emit flashDataReady(data);
            break;
        default:
            QMessageBox::critical(NULL, "错误", "接受到错误的UDP数据包");
            break;
    }
}

/**
 * @brief 发送长度固定的系统参数
 * @return
 */
void ProtocolDispatch::encode(qint32 command, qint32 data_len, qint32 data)
{
    QByteArray frame;
    uint32_t   checksum = 0xeeeeffff;

    frame.append(QByteArray::fromHex("AA555AA5AA555AA5"));
    frame.append(QByteArray::fromHex(QByteArray::number(cmdNum++, 16).rightJustified(8, '0')));
    frame.append(QByteArray::fromHex(QByteArray::number(command, 16).rightJustified(8, '0')));
    frame.append(QByteArray::fromHex(QByteArray::number(packetNum, 16).rightJustified(8, '0')));
    frame.append(QByteArray::fromHex(QByteArray::number(data_len, 16).rightJustified(8, '0')));
    frame.append(QByteArray::fromHex(QByteArray::number(data, 16).rightJustified(8, '0').append(504, '0')));
    frame.append(QByteArray::fromHex(QByteArray::number(checksum, 16).rightJustified(8, '0')));

    emit frameDataReady(frame);
}

void ProtocolDispatch::encode(qint32 command, qint32 data_len, QByteArray &data)
{
    QByteArray frame;
    uint32_t   checksum = 0xeeeeffff;
    frame.append(QByteArray::fromHex("AA555AA5AA555AA5"));
    frame.append(QByteArray::fromHex(QByteArray::number(cmdNum++, 16).rightJustified(8, '0')));
    frame.append(QByteArray::fromHex(QByteArray::number(command, 16).rightJustified(8, '0')));
    frame.append(QByteArray::fromHex(QByteArray::number(packetNum, 16).rightJustified(8, '0')));
    frame.append(QByteArray::fromHex(QByteArray::number(data_len, 16).rightJustified(8, '0')));
    frame.append(data);
    if(data_len < 256)
        frame.append(256 - data_len, 0);
    else
        QMessageBox::warning(nullptr, "警告", "需要打包的数据过长");
    frame.append(QByteArray::fromHex(QByteArray::number(checksum, 16).rightJustified(8, '0')));
    emit frameDataReady(frame);
}
