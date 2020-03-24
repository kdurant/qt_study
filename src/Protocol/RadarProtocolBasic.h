#ifndef RADARPROTOCOLBASIC_H
#define RADARPROTOCOLBASIC_H
#include <QtCore>

/*
 * 帧头：      偏移地址，0；长度，8
 * 指令序号：   偏移地址，8；长度，4
 * 指令：      偏移地址，12；长度，4
 * 包序号：     偏移地址，16；长度，4
 * 有效数据长度：偏移地址，20；长度，4
 * 数据：      偏移地址，24；长度，256
 * 教研：      偏移地址，280；长度，4
 * /

/**
 * @brief
 */

#define COMMAND_POS 12
#define COMMAND_LEN 4
#define PCK_NUM_POS 16
#define PCK_NUM_LEN 4
#define VALID_DATA_LEN_POS 20
#define VALID_DATA_LEN_LEN 4

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
    QByteArray sys_para;
} ProtocolResult;

/**
  包括数据，和坐标点信息
  */
typedef struct __OneChData
{
    QVector<qint32> Coor;
    QByteArray      Data;  // 坐标的2倍长度
} OneChData;

typedef struct __WaveData
{
    bool      isEmpty;
    OneChData ch[4];
} WaveData;

class RadarProtocolBasic
{
protected:
    QQueue<QByteArray> waveFrame;     // 存放采样数据原始数据帧
    quint32            waveCntLast;   // 上次收到采样数据的包序号
    quint32            waveCntCur;    // 本次收到采样数据的包序号
    quint32            waveFrameCnt;  // 队列里存放了几个完整的采样数据帧

    QByteArray commandFrame;  // 存放控制命令原始数据帧
    quint8     processFlag;   // 本次数据是否需要处理

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

        waveCntLast  = 0;
        waveCntCur   = 0;
        waveFrameCnt = 0;
        processFlag  = 0;
    }
    virtual ~RadarProtocolBasic()
    {
    }

    void                setDataFrame(QByteArray &originFrame);
    quint32             getCmdNum(QVector<quint8> &dataFrame);
    quint32             getCmdData(QVector<quint8> &dataFrame);
    QByteArray          encode(qint32 command, qint32 data_len, qint32 data);
    ProtocolResult      getFPGAInfo();
    virtual QByteArray &removeChNeedlessInfo(QByteArray &data) = 0;
    virtual WaveData    getSignalWave()                        = 0;
};

#endif  // RADARPROTOCOLBASIC_H
