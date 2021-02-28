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
    };

    /**
     * @brief 整型数据转换为length=4的QByteArray
     * @param data
     * @return 
     */
    static QByteArray int2ba(uint32_t data)
    {
        QByteArray ba;
        ba.resize(4);
        ba[3] = static_cast<int8_t>(0x000000ff & data);
        ba[2] = static_cast<int8_t>((0x0000ff00 & data) >> 8);
        ba[1] = static_cast<int8_t>((0x00ff0000 & data) >> 16);
        ba[0] = static_cast<int8_t>((0xff000000 & data) >> 24);
        return ba;
    }

    static qint32 ba2int(QByteArray ba)
    {
        qint32 ret = (ba[0] << 24) + (ba[1] << 16) + (ba[2] << 8) + (ba[3] << 0);
        return ret;
    }
};

#endif  // BSP_CONFIG_H
