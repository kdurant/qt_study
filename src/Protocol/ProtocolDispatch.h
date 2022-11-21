#ifndef PROTOCOL_DISPATCH_H
#define PROTOCOL_DISPATCH_H

#include <QtCore>
#include "protocol.h"

class FrameField
{
public:
    enum fieldOffset
    {
        COMMAND_POS        = 12,
        COMMAND_LEN        = 4,
        PCK_NUM_POS        = 16,
        PCK_NUM_LEN        = 4,
        VALID_DATA_LEN_POS = 20,
        VALID_DATA_LEN_LEN = 4,
        DATA_POS           = 24,
        DATA_LEN           = 256
    };
};

/**
 * @brief 功能如下：
 * 1. 解析数据帧的命令字段内容，发射相应的信号给具体的处理模块
 * 2. 提供一个槽函数，需要按照协议打包数据的模块，可以通过信号将数据发送进来
 * 3. 数据打包后，发送信号，通知UDP将数据发送出去
 */
class ProtocolDispatch : public QObject
{
    Q_OBJECT
public:
    ProtocolDispatch()
    {
        //        cmdNum = 0;
    }
    ~ProtocolDispatch()
    {
    }

    void parserFrame(QByteArray &data);

    static uint32_t getCommand(QByteArray &data)
    {
        return data.mid(FrameField::COMMAND_POS, FrameField::COMMAND_LEN).toHex().toUInt(nullptr, 16);
    }
    static int getPckNum(QByteArray &data)
    {
        return data.mid(FrameField::PCK_NUM_POS, FrameField::PCK_NUM_LEN).toHex().toInt(nullptr, 16);
    }
    static int getDataLen(QByteArray &data)
    {
        return data.mid(FrameField::VALID_DATA_LEN_POS, FrameField::VALID_DATA_LEN_LEN).toHex().toInt(nullptr, 16);
    }

public slots:
    void encode(qint32 command, qint32 data_len, QByteArray data);

signals:
    /**
     * @brief 系统状态不转发，直接返回
     * @param data
     */
    void infoDataReady(const QByteArray &data);

    /**
     * @brief 通过信号将数据传递给预览数据处理模块
     * @param data
     */
    void onlineDataReady(QByteArray &data);
    void flashDataReady(QByteArray &data);
    void ssdDataReady(QByteArray &data);

    void gpsFrameReady(const QByteArray &data);
    void laserDataReady(const QByteArray &data);
    void motorDataReady(QByteArray &data);
    void attitudeDataReady(QByteArray &data);
    void ms5837DataReady(QByteArray &data);
    void ADDataReady(QByteArray &data);
    void DADataReady(QByteArray &data);

    void errorDataReady(QString &data);

    void remoteUpdateDataReady(QByteArray data);

    /**
     * @brief 数据封包已经完成，可以通过UDP发送出去
     */
    void frameDataReady(QByteArray data);

private:
    QVector<quint8>  head;
    static quint32   cmdNum;
    quint32          cmdData;
    quint32          packetNum{0};
    quint32          validDataLen;
    QVector<quint16> data;
    quint32          checksum;

    QString deviceVersion;
};
#endif
