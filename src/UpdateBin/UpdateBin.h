#ifndef UPDATE_BIN_H
#define UPDATE_BIN_H

#include "protocol.h"
#include <QMessageBox>
#include <QtCore>

class UpdateBin : public QObject
{
    Q_OBJECT
public:
    enum
    {
        BYTES_PER_WRITE  = 256,
        FLASH_BLOCK_SIZE = 0x10000 * 2,  // 64Kword/block,  128K Byte/block
        BIN_FILE_OFFSET  = 0x0000000
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

    void flashErase(uint32_t addr);

    /**
     * @brief 读取起始地址后的256个字节数据
     * @param addr
     * @return
     */
    QByteArray flashRead(uint32_t addr);

    /**
     * @brief writeFlash
     * 需要依次完成发送flash数据， 要写入flash地址，自动启动flash写操作
     * @param addr
     * @param data
     */
    void flashWrite(uint32_t addr, QByteArray& data);

    bool flashUpdate(QString& filePath);

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
