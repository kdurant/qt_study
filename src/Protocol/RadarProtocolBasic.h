#ifndef RADARPROTOCOLBASIC_H
#define RADARPROTOCOLBASIC_H
#include <QtCore>

enum commandSet
{
    LASER_DOWN_DATA = 0x000000001,
    LASER_FREQ      = 0x000000002,
    MOTOR_DOWN_DATA = 0x000000003,
    SET_DAC,
    READ_ADC,
    RESERVE_UART1
};

class RadarProtocolBasic
{
private:
    QVector<quint8>  dataFrame;
    QVector<quint8>  head;
    quint32          cmdNum;
    quint32          cmdData;
    quint32          packetNum;
    quint32          validDataLen;
    QVector<quint16> data;

public:
    RadarProtocolBasic();
    virtual ~RadarProtocolBasic()
    {
    }

    QVector<quint8>          setDataFrame(QVector<quint8> &dataFrame);
    QVector<quint8>          setDataFrame(QQueue<quint8> &dataFrame);
    quint32                  getCmdNum(QVector<quint8> &dataFrame);
    quint32                  getCmdData(QVector<quint8> &dataFrame);
    virtual QVector<quint16> getSignalWave(QVector<quint8>) const = 0;
};

#endif  // RADARPROTOCOLBASIC_H
