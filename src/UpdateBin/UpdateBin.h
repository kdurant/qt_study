#ifndef UPDATE_BIN_H
#define UPDATE_BIN_H
#include "common.h"
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
        BIN_FILE_OFFSET  = 0x6000000     // MCS文件烧录到这个地址0x6000000
    };
    UpdateBin()
    {
        isRecvFlashData = false;
    }
    ~UpdateBin()
    {
    }

    void flashErase(uint32_t addr);

    QByteArray pageRead(uint32_t addr);
    QByteArray pageReadWithCheck(uint32_t addr);

    void pageWrite(uint32_t addr, QByteArray& data);

    bool blockWrite(uint32_t addr, QByteArray& data);

    bool checkBinFormat(QString& filePath);
    bool flashUpdate(QString& filePath);

public slots:
    void setDataFrame(QByteArray& frame);

signals:
    void flashCommandReadySet(uint32_t command, uint32_t data_len, QByteArray data);
    void flashCommandReadySet1();
    void recvFlashData();
    // 已经更新的字节数
    void updatedBytes(uint32_t bytes);

private:
    QByteArray frame;
    bool       isRecvFlashData;

    QByteArray readData;
    QByteArray writeData;

    QByteArray int2ba(uint32_t data);
};

#endif
