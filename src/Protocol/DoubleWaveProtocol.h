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
    virtual QByteArray& removeChNeedlessInfo(QByteArray& data);
    virtual WaveData    getSignalWave();
};

#endif  // DOUBLEWAVEPROTOCOL_H
