#ifndef BSP_CONFIG_H
#define BSP_CONFIG_H
#include <QtCore>

class BspConfig
{
public:
    enum RadarType
    {
        RADAR_TPYE_OCEAN       = 0x00,
        RADAR_TPYE_LAND        = 0x01,
        RADAR_TPYE_760         = 0x02,
        RADAR_TPYE_DOUBLE_WAVE = 0x03,
        RADAR_TPYE_DRONE       = 0x04,
        RADAR_TPYE_UNDER_WATER = 0x05,
    };

    struct Gps_Info
    {
        qint32 week;
        qint32 current_week_ms;
        double latitude;
        double longitude;
        double altitude;
        double roll;
        double pitch;
        double heading;
    };

    /**
     * @brief 整型数据转换为length=4的QByteArray
     * @param data
     * @return 
     */
    static QByteArray
    int2ba(quint32 data)
    {
        QByteArray ba;
        ba.resize(4);
        ba[3] = static_cast<int8_t>(0x000000ff & data);
        ba[2] = static_cast<int8_t>((0x0000ff00 & data) >> 8);
        ba[1] = static_cast<int8_t>((0x00ff0000 & data) >> 16);
        ba[0] = static_cast<int8_t>((0xff000000 & data) >> 24);
        return ba;
    }

    static quint32 ba2int(QByteArray ba)
    {
        quint32 ret = ba.toHex().toUInt(nullptr, 16);
        return ret;
    }
};

#endif  // BSP_CONFIG_H
