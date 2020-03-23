#ifndef DOUBLEWAVEPROTOCOL_H
#define DOUBLEWAVEPROTOCOL_H
#include "RadarProtocolBasic.h"

class DoubleWaveProtocol : public RadarProtocolBasic
{
public:
    DoubleWaveProtocol()
        : RadarProtocolBasic()
    {
    }
    virtual ~DoubleWaveProtocol()
    {
    }
    virtual QVector<quint16> getSignalWave(QVector<quint8>);
};

#endif  // DOUBLEWAVEPROTOCOL_H
