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
    QTimer::singleShot(1000, &loop, SLOT(quit()));
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
 * @brief UpdateBin::pageReadWithCheck
 * 同一地址连续读取两次，两次数据一致，才返回正确数据，否则最多三次重读
 * 三次重读都错误时，返回256个0xee
 * @param addr
 * @return
 */
QByteArray UpdateBin::pageReadWithCheck(uint32_t addr)
{
    qint32 reReadCnt = 0;

    while(reReadCnt < 3)
    {
        QByteArray data1 = pageRead(addr);
        QByteArray data2 = pageRead(addr);
        if(data1 == data2)
        {
            reReadCnt = 0;
            return data1;
        }
        else
        {
            reReadCnt++;
            qDebug() << "Single read data error, addr is at: " << addr;
        }
    }
    qDebug() << "Final read data error, addr is at: " << addr;
    return QByteArray(256, 0xee);
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

    QByteArray writeData;
    QByteArray recvData;
    flashErase(addr);

    for(int i = 0; i < FLASH_BLOCK_SIZE / 256; i++)
    {
        writeData = data.mid(i * 256, 256);
        pageWrite(addr + i * 256, writeData);
        //        recvData = pageRead(addr + i * 256);
        recvData = pageReadWithCheck(addr + i * 256);
        if(writeData != recvData)
        {
            qDebug() << "The addr that pageWrite() generata error is at: " << addr + i * 256;
            return false;
        }
    }
    return true;
}

bool UpdateBin::checkBinFormat(QString &filePath)
{
    QVector<quint8> data{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xBB, 0x11, 0x22, 0x00, 0x44, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAA, 0x99, 0x55, 0x66, 0x20, 0x00, 0x00, 0x00, 0x30, 0x03, 0xE0, 0x01, 0x00, 0x00, 0x00, 0x0B};
    QByteArray      headFormat;
    for(auto i : data)
        headFormat.append(i);

    QFile file(filePath);
    file.open(QIODevice::ReadOnly);

    bool status = false;
    if(headFormat == file.read(data.size()))
        status = true;
    file.close();
    return status;
}

bool UpdateBin::flashUpdate(QString &filePath)

{
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    uint32_t hasWriteBytes    = 0;
    qint32   reSendCnt        = 0;
    bool     blockWriteStatus = false;

    bool updateStatus = true;

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
            blockWriteStatus = blockWrite(BIN_FILE_OFFSET + hasWriteBytes, writeData);

            if(blockWriteStatus)
            {
                reSendCnt = 0;
                break;
            }
            else
            {
                reSendCnt++;
                qDebug() << "reSentCnt is " << reSendCnt;
                qDebug() << "The addr that blockWrite() generata error is at: " << BIN_FILE_OFFSET + hasWriteBytes;
            }
        }
        hasWriteBytes += len;
        emit updatedBytes(hasWriteBytes);
        if(reSendCnt > 3)
        {
            updateStatus = false;
            reSendCnt    = 0;

            break;
        }
    }
    emit updatedBytes(hasWriteBytes);

    return updateStatus;
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
