#ifndef BSP_CONFIG_H
#define BSP_CONFIG_H

class BspConfig
{
public:
    enum RadarType
    {
        RADAR_TPYE_OCEAN       = 0x00,
        RADAR_TPYE_LAND        = 0x01,
        RADAR_TPYE_760         = 0x02,
        RADAR_TPYE_DOUBLE_WAVE = 0x03,
    };
};

#endif  // BSP_CONFIG_H
