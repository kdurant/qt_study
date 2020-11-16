#ifndef UPDATE_BIN_H
#define UPDATE_BIN_H

#include <QtCore>
#include "src/Protocol/protocol.h"

class UpdateBin : public QObject
{
    Q_OBJECT
public:
    enum
    {
        BYTES_PER_WRITE  = 128,
        FLASH_BLOCK_SIZE = 0x10000
    };
    UpdateBin()
    {
        isRecvFlashData = false;
    }
    ~UpdateBin()
    {
    }
    void setFlashAddr(uint32_t addr)
    {
        opFlashAddr = addr;
    }

    void flashErase(uint32_t addr)
    {
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
    QByteArray flashRead(uint32_t addr)
    {
        QByteArray ba = int2ba(addr);
        emit       flashCommandReadySet(MasterSet::READ_ADDR, 4, ba);
        QEventLoop waitLoop;
        connect(this, &UpdateBin::recvFlashData, &waitLoop, &QEventLoop::quit);
        QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
        waitLoop.exec();
        isRecvFlashData = false;
        return readData;
    }
    /**
     * @brief writeFlash
     * 需要依次完成发送flash地址， 要写入flash的数据，启动flash写操作
     * @param addr
     * @param data
     */
    void flashWrite(uint32_t addr, QByteArray& data)
    {
        writeData     = data;
        QByteArray ba = int2ba(addr);
        emit       flashCommandReadySet(MasterSet::WRITE_ADDR, 4, ba);
        emit       flashCommandReadySet(MasterSet::WRITE_DATA, data.size(), data);
        ba = int2ba(0x11111111);
        emit flashCommandReadySet(MasterSet::WRITE_RUN, 4, ba);
    }

    bool flashUpdate(QString& filePath)
    {
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        uint32_t   hasWriteBytes = 0;
        QByteArray writeData;
        QByteArray secondData;

        while(!file.atEnd())
        {
            writeData = file.read(UpdateBin::BYTES_PER_WRITE);
            if(hasWriteBytes % UpdateBin::FLASH_BLOCK_SIZE == 0)
            {
                //            flashErase(hasWriteBytes);
            }

            flashWrite(hasWriteBytes, writeData);
            secondData = flashRead(hasWriteBytes);

            // 只有最后一次才会出现这种情况
            if(hasWriteBytes != UpdateBin::BYTES_PER_WRITE)
            {
                return writeData == readData.mid(0, UpdateBin::BYTES_PER_WRITE);
            }
            else
            {
                if(writeData != readData)
                    return false;
            }
        }
        return true;
    }

public slots:
    void setDataFrame(QByteArray& frame);

signals:
    void flashCommandReadySet(uint32_t command, uint32_t data_len, QByteArray& data);
    void flashCommandReadySet1();
    void recvFlashData();

private:
    QByteArray frame;
    bool       isRecvFlashData;
    uint32_t   opFlashAddr;  // 读、写、擦除  共用一个地址

    QByteArray readData;
    QByteArray writeData;

    QByteArray int2ba(uint32_t data);
};

#endif
