#ifndef SAVEWAVE_H
#define SAVEWAVE_H
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
class SaveWave : public QObject
{
    Q_OBJECT

private:
    QVector<QByteArray> allData;
    QTimer *timer;

public:
    struct ValidFileInfo
    {
        QByteArray name;
        qint32 startUnit;
        qint32 endUnit;
    };

public:
    SaveWave()
    {
        timer = new QTimer();
        timer->setInterval(20);
    }
    bool readDiskUnit(qint32 unitAddr, QByteArray &ret);
    bool inquireSpace(qint32 startUnit, ValidFileInfo &fileInfo);

    bool setSaveFileName(QByteArray &name);

signals:
    void sendDataReady(qint32 command,
                       qint32 data_len,
                       QByteArray &data); // 需要发送的数据已经准备好

public slots:

    void setNewData(QByteArray &data)
    {
        allData.append(data);
        timer->start();
    }
};
#endif
