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
 * @brief ProtocolDispatch, 根据数据帧的命令字段内容，发射相应的信号给具体的处理模块
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

    /**
     * @brief 通过信号将收到的数据分发给具体处理的模块
     * @param data
     */
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

public:
    QByteArray encode(qint32 command, qint32 data_len, qint32 data);
    QByteArray encode(qint32 command, qint32 data_len, QByteArray &data);

    QString getDeviceVersion() { return deviceVersion; }

signals:
    /**
     * @brief 系统状态不转发，直接返回
     * @param data
     */
    void infoDataReady(uint32_t para, QByteArray &data);

    /**
     * @brief 通过信号将数据传递给预览数据处理模块
     * @param data
     */
    void previewDataReady(QByteArray &data);
    void flashDataReady(QByteArray &data);

    void remoteUpdateDataReady(QByteArray data);

private:
    QVector<quint8>  head;
    static quint32   cmdNum;
    quint32          cmdData;
    quint32          packetNum;
    quint32          validDataLen;
    QVector<quint16> data;
    quint32 checksum;

    QString deviceVersion;
};
#endif
