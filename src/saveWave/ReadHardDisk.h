#ifndef READHARDDISK_H
#define READHARDDISK_H
#include <QQueue>
#include <QtCore>

#include "ProtocolDispatch.h"
#include "bsp_config.h"
#include "protocol.h"

/*
 * SSD 16KBytes存储空间为一个基本unit
 * 基数unit前面252个字节保存文件名称，后面填充0xee
 * 偶数unit保存文件的起始地址（4Bytes）和结束地址（4Bytes），后面填充0xdd
 */
class ReadHardDisk : public QObject
{
    Q_OBJECT

public:
    enum FileStatus
    {
        FILE_INFO_FULL = 0x00,
        FILE_POSITION_ERROR,  // 文件名正确，文件位置信息错误
        FILE_INFO_NONE,       // 没有收到全部数据包
    };

    struct ValidFileInfo
    {  // 保存最近一次有效检索文件的信息
        FileStatus status;
        QByteArray name;
        qint32     fileUnit;   // 本文件 文件名在ssd中的存储位置
        qint32     startUnit;  // 本文件在ssd中的起始位置
        qint32     endUnit;    // 本文件在ssd中的结束位置
    };

private:
    QVector<QByteArray>  allData;
    QTimer              *timer;
    QList<ValidFileInfo> fileList;

public:
    ReadHardDisk()
    {
        timer = new QTimer();
        timer->setInterval(5);
    }
    bool                 readDiskUnit(qint32 unitAddr, QByteArray &ret);
    QVector<QByteArray> &readDiskUnit(qint32 unitAddr);
    bool                 inquireSpace(qint32 startUnit, ValidFileInfo &fileInfo);

    QList<ValidFileInfo> getFileList(void)
    {
        return fileList;
    }

    bool setSaveFileName(quint32 unit, QString &name);
    bool setSaveFileAddr(quint32 unit);
    bool enableStoreFile(quint32 status);

signals:
    void sendDataReady(qint32      command,
                       qint32      data_len,
                       QByteArray &data);  // 需要发送的数据已经准备好

    void fileDataReady(ValidFileInfo &fileInfo);

public slots:
    // data为通信协议中的完整数据帧
    void setNewData(QByteArray &data)
    {
        allData.append(data);
        timer->start();
    }
};
#endif
