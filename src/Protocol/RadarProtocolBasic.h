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
    LASER_DOWN_DATA = 0x000000001,
    LASER_FREQ      = 0x000000002,
    MOTOR_DOWN_DATA = 0x000000003,
    SET_DAC,
    READ_ADC,
    RESERVE_UART1
};

enum RecvCommandSet
{
    AD_SAMPLE_DATA = 0x80000006
};

/*
 *
 */
typedef struct __ProtocolResult
{
    quint32 cmdData;
    quint32 data;
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
    RadarProtocolBasic();
    virtual ~RadarProtocolBasic()
    {
    }

    void                     setDataFrame(QByteArray &originFrame);
    quint32                  getCmdNum(QVector<quint8> &dataFrame);
    quint32                  getCmdData(QVector<quint8> &dataFrame);
    QByteArray               encode(qint32 command, qint32 data_len, qint32 data);
    virtual QVector<quint16> getSignalWave(QVector<quint8>) const = 0;
};

#endif  // RADARPROTOCOLBASIC_H
