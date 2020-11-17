#ifndef UPDATE_BIN_H
#define UPDATE_BIN_H

#include <QtCore>
#include <QMessageBox>
#include "src/Protocol/protocol.h"

class UpdateBin : public QObject
{
    Q_OBJECT
public:
    enum
    {
        BYTES_PER_WRITE  = 256,
        FLASH_BLOCK_SIZE = 0x10000,
        BIN_FILE_OFFSET  = 0x4000000
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
        QTimer::singleShot(2000, &loop, SLOT(quit()));
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
        QTimer::singleShot(5, &waitLoop, &QEventLoop::quit);
        waitLoop.exec();
        isRecvFlashData = false;
        return readData;
    }
    /**
     * @brief writeFlash
     * 需要依次完成发送flash数据， 要写入flash地址，自动启动flash写操作
     * @param addr
     * @param data
     */
    void flashWrite(uint32_t addr, QByteArray& data)
    {
        writeData     = data;
        QByteArray ba = int2ba(addr);

        emit       flashCommandReadySet(MasterSet::WRITE_DATA, data.size(), data);
        QEventLoop waitLoop;
        QTimer::singleShot(1, &waitLoop, &QEventLoop::quit);
        waitLoop.exec();

        emit flashCommandReadySet(MasterSet::WRITE_ADDR, 4, ba);
        QTimer::singleShot(5, &waitLoop, &QEventLoop::quit);
        waitLoop.exec();
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
            int len   = writeData.length();
            if(hasWriteBytes % UpdateBin::FLASH_BLOCK_SIZE == 0)
            {
                flashErase(BIN_FILE_OFFSET + hasWriteBytes);
            }
            auto swapByteOrder = [&writeData](int len) {
                for(int i = 0; i < len; i += 2)
                {
                    uint8_t temp;
                    temp             = writeData[i];
                    writeData[i]     = writeData[i + 1];
                    writeData[i + 1] = temp;
                }
            };
            swapByteOrder(len);

            flashWrite(BIN_FILE_OFFSET + hasWriteBytes, writeData);
            secondData = flashRead(BIN_FILE_OFFSET + hasWriteBytes);

            // 只有最后一次才会出现这种情况
            if(len != UpdateBin::BYTES_PER_WRITE)
            {
                return writeData == readData.mid(0, len);
            }
            else
            {
                if(writeData != readData)
                {
                    QMessageBox::critical(NULL, "critical", "写入数据错误");
                    return false;
                }
            }
            hasWriteBytes += len;
            emit updatedBytes(hasWriteBytes);
        }
        return true;
    }

public slots:
    void setDataFrame(QByteArray& frame);

signals:
    void flashCommandReadySet(uint32_t command, uint32_t data_len, QByteArray& data);
    void flashCommandReadySet1();
    void recvFlashData();
    // 已经更新的字节数
    void updatedBytes(uint32_t bytes);

private:
    QByteArray frame;
    bool       isRecvFlashData;
    uint32_t   opFlashAddr;  // 读、写、擦除  共用一个地址

    QByteArray readData;
    QByteArray writeData;

    QByteArray int2ba(uint32_t data);
};

#endif
