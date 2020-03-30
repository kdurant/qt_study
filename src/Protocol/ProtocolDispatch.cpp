#include "ProtocolDispatch.h"
#include <QMessageBox>

quint32 ProtocolDispatch::cmdNum = 0;

void ProtocolDispatch::dipatchData(QByteArray &data)
{
    uint32_t command = getCommand(data);
    switch(command)
    {
        case SlaveUp::SYS_INFO:
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
        default:
            QMessageBox::critical(NULL, "错误", "接受到错误的UDP数据包");
            break;
    }
}

QByteArray ProtocolDispatch::encode(qint32 command, qint32 data_len, qint32 data)
{
    QByteArray frame;
    QByteArray origin;
    qint32     checksum = 0xeeeeffff;
    origin.append("AA555AA5AA555AA5");
    origin.append(QByteArray::number(cmdNum++, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(command, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(packetNum, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(data_len, 16).rightJustified(8, '0'));
    origin.append(QByteArray::number(data, 16).rightJustified(8, '0').append(504, '0'));
    origin.append(QByteArray::number(checksum, 16).rightJustified(8, '0'));
    frame = QByteArray::fromHex(origin);

    return frame;
}
