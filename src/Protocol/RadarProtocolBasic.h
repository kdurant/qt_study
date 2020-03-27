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
class MASTER_SET
{
public:
    enum send
    {
        PREVIEW_ENABLE = 0x0000000C,
        SYS_INFO       = 0x00000013,

        SAMPLE_LEN     = 0x00000008,
        PREVIEW_RATIO  = 0x00000009,
        FIRST_POS      = 0x00000029,
        FIRST_LEN      = 0x0000002a,
        SECOND_POS     = 0x0000002e,
        SECOND_LEN     = 0x00000030,
        SUM_THRE       = 0x0000002b,
        MAX_THRE       = 0x0000002c,
        COMPRESS_LEN   = 0x00200001,
        COMPRESS_RATIO = 0x00200002,
    };
};

class SLAVE_UP
{
public:
    enum recv
    {
        SYS_INFO     = 0x80000001,
        PREVIEW_DATA = 0x80000006
    };
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
    QQueue<QByteArray> originWaveFrame;  // 存放采样数据原始数据帧
    quint32            waveCntLast;      // 上次收到采样数据的包序号
    quint32            waveFrameCnt;     // 队列里存放了几个完整的采样数据帧

    QByteArray commandFrame;     // 存放控制命令原始数据帧
    quint8     hasCommandFrame;  // 本次数据是否需要处理

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

        waveCntLast     = 0;
        waveFrameCnt    = 0;
        hasCommandFrame = 0;
    }
    virtual ~RadarProtocolBasic()
    {
    }

    void                setDataFrame(QByteArray &originFrame);
    int                 getWaveFrameCnt();
    quint32             getCmdNum(QVector<quint8> &dataFrame);
    quint32             getCmdData(QVector<quint8> &dataFrame);
    QByteArray          encode(qint32 command, qint32 data_len, qint32 data);
    ProtocolResult      getFPGAInfo();
    virtual QByteArray &removeChNeedlessInfo(QByteArray &data) = 0;
    virtual WaveData    getSignalWave()                        = 0;
};

#endif  // RADARPROTOCOLBASIC_H
