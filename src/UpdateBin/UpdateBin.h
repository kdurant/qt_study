#ifndef UPDATE_BIN_H
#define UPDATE_BIN_H

#include <QtCore>
#include <QUdpSocket>

class UpdateBin
{
public:
    enum command
    {
        ERASE_BLOCK      = 0x00100000,
        READ_BLOCK       = 0x00100001,
        WRITE_BLOCK_ADDR = 0x00100002,
        WRITE_BLOCK_DATA = 0x00100003,
        WRITE_BLOCK_RUN  = 0x00100004,

        RES_FLASH_DATA = 0x80100002
    };

    UpdateBin(QUdpSocket* s)
        : udpSocked(s)
    {
    }
    ~UpdateBin()
    {
    }

    bool       eraseFlase(int addr);
    QByteArray readFlash(int addr);
    bool       writeFlash(int addr, QByteArray& data);
    bool       updateBin(QFile& filePath);

private:
    QUdpSocket* udpSocked;
    QByteArray  readData;
    QByteArray  writeData;
};

#endif
