#include <QEventLoop>
#include <QFile>
#include "src/Protocol/ProtocolDispatch.h"
#include "UpdateBin.h"

void UpdateBin::setDataFrame(QByteArray &frame)
{
    if(ProtocolDispatch::getCommand(frame) == SlaveUp::FLASH_DATA)
    {
        isRecvFlashData = true;
        readData        = frame;
    }
    else
        isRecvFlashData = false;
}

QByteArray UpdateBin::int2ba(uint32_t data)
{
    QByteArray ba;
    ba.resize(4);
    ba[3] = static_cast<int8_t>(0x000000ff & data);
    ba[2] = static_cast<int8_t>((0x0000ff00 & data) >> 8);
    ba[1] = static_cast<int8_t>((0x00ff0000 & data) >> 16);
    ba[0] = static_cast<int8_t>((0xff000000 & data) >> 24);
    //    QByteArray::fromHex(QByteArray::number(data, 16))
    return ba;
}
