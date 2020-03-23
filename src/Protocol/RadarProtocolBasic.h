#ifndef RADARPROTOCOLBASIC_H
#define RADARPROTOCOLBASIC_H
#include <QtCore>
/**
 * @brief
 */

#define COMMAND_POS 12
#define COMMAND_LEN 4

//#define PCK_NUMER_POS 32
//#define PCK_NUMBER_LEN 8
//#define VALID_LEN_POS 40
//#define VALID_LEN_LEN 8

enum SendCommandSet
{
    PC_SET_PREVIEW_ENABLE = 0x0000000C,
    PC_READ_SYS_INFO      = 0x00000013,
    SET_DAC,
    READ_ADC,
    RESERVE_UART1
};

enum RecvCommandSet
{
    READ_SYS_INFO  = 0x80000001,
    AD_SAMPLE_DATA = 0x80000006
};

/*
 *
 */
typedef struct __ProtocolResult
{
    QByteArray cmdData;
    QByteArray data;
} ProtocolResult;

class RadarProtocolBasic
{
protected:
    QQueue<QByteArray> waveFrame;     // 存放采样数据原始数据帧
    QByteArray         commandFrame;  // 存放控制命令原始数据帧
    quint8             processFlag;   // 本次数据是否需要处理
    bool               waveFlag;

    QVector<quint8>  dataFrame;
    QVector<quint8>  head;
    quint32          cmdNum;
    quint32          cmdData;
    quint32          packetNum;
    quint32          validDataLen;
    QVector<quint16> data;
    quint32          checksum;

public:
    RadarProtocolBasic()
    {
        head         = {0xAA, 0x55, 0x5A, 0xA5, 0xAA, 0x55, 0x5A, 0xA5};
        cmdNum       = 0;
        cmdData      = 0;
        packetNum    = 0;
        validDataLen = 0;
        data         = {256, 0};

        processFlag = 0;
        waveFlag    = false;
    }
    virtual ~RadarProtocolBasic()
    {
    }

    void                     setDataFrame(QByteArray &originFrame);
    quint32                  getCmdNum(QVector<quint8> &dataFrame);
    quint32                  getCmdData(QVector<quint8> &dataFrame);
    QByteArray               encode(qint32 command, qint32 data_len, qint32 data);
    ProtocolResult           getFPGAInfo();
    virtual QVector<quint16> getSignalWave(QVector<quint8>) = 0;
};

#endif  // RADARPROTOCOLBASIC_H
