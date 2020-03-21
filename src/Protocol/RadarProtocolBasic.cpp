#include "RadarProtocolBasic.h"

RadarProtocolBasic::RadarProtocolBasic()
{
    head         = {0xAA, 0x55, 0x5A, 0xA5, 0xAA, 0x55, 0x5A, 0xA5};
    cmdNum       = 0;
    cmdData      = 0;
    packetNum    = 0;
    validDataLen = 0;
    data         = {256, 0};
}
