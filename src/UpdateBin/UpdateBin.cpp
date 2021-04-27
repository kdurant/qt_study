#include <QEventLoop>
#include <QFile>
#include "ProtocolDispatch.h"
#include "UpdateBin.h"

/**
 * @brief 擦除nor flash扇区时，好像可以输入本扇区内的任意地址都可以（有机会重新确认）
 * @param addr
 */
void UpdateBin::flashErase(uint32_t addr)
{
    addr /= 2;
    QByteArray ba = int2ba(addr);
    emit       flashCommandReadySet(MasterSet::ERASE_ADDR, 4, ba);
    QEventLoop loop;
    QTimer::singleShot(1200, &loop, SLOT(quit()));
    loop.exec();
}

/**
 * @brief 读取起始地址后的256个字节数据
 * @param addr
 * @return
 */
QByteArray UpdateBin::pageRead(uint32_t addr)
{
    addr /= 2;
    QByteArray ba = int2ba(addr);
    emit       flashCommandReadySet(MasterSet::READ_ADDR, 4, ba);
    QEventLoop waitLoop;
    connect(this, &UpdateBin::recvFlashData, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(5, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();
    isRecvFlashData = false;
    return readData;
}

/**
 * @brief pageWrite
 * 需要依次完成发送flash数据， 要写入flash地址，自动启动flash写操作
 * @param addr
 * @param data
 */
void UpdateBin::pageWrite(uint32_t addr, QByteArray &data)
{
    addr /= 2;
    QByteArray ba = int2ba(addr);

    emit       flashCommandReadySet(MasterSet::WRITE_DATA, data.size(), data);
    QEventLoop waitLoop;
    QTimer::singleShot(1, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();

    emit flashCommandReadySet(MasterSet::WRITE_ADDR, 4, ba);
    QTimer::singleShot(5, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();
}

/**
 * @brief UpdateBin::blockWrite
 * PC28F00一个block是128K Byte
 * @param addr
 * @param data
 */
bool UpdateBin::blockWrite(uint32_t addr, QByteArray &data)
{
    if(data.size() != FLASH_BLOCK_SIZE)
        return false;

    qDebug() << data.size();
    QByteArray writeData;
    QByteArray recvData;
    flashErase(addr);

    for(int i = 0; i < FLASH_BLOCK_SIZE / 256; i++)
    {
        writeData = data.mid(i * 256, 256);
        pageWrite(addr + i * 256, writeData);
        recvData = pageRead(addr + i * 256);
        if(writeData != recvData)
            return false;
    }
    return true;
}

bool UpdateBin::flashUpdate(QString &filePath)

{
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    uint32_t hasWriteBytes = 0;
    qint32   reSendCnt     = 0;
    bool     status        = false;

    while(!file.atEnd())
    {
        writeData = file.read(UpdateBin::FLASH_BLOCK_SIZE);
        int len   = writeData.length();
        if(len < UpdateBin::FLASH_BLOCK_SIZE)
        {
            writeData.append(QByteArray(UpdateBin::FLASH_BLOCK_SIZE - len, 0xee));
        }
        for(auto &i : writeData)
            i = Common::bitSwap(i);

        while(reSendCnt <= 3)
        {
            status = blockWrite(BIN_FILE_OFFSET + hasWriteBytes, writeData);
            if(status)
            {
                reSendCnt = 0;
                break;
            }
            else
                reSendCnt++;
        }

        hasWriteBytes += len;
        emit updatedBytes(hasWriteBytes);
    }
    emit updatedBytes(hasWriteBytes);
    return true;
}

void UpdateBin::setDataFrame(QByteArray &frame)
{
    if(ProtocolDispatch::getCommand(frame) == SlaveUp::FLASH_DATA)
    {
        isRecvFlashData = true;
        readData        = frame.mid(FrameField::DATA_POS, FrameField::DATA_LEN);
        emit recvFlashData();
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
