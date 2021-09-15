#ifndef BSP_CONFIG_H
#define BSP_CONFIG_H
#include <QtCore>
#define CONCAT_(x, y) x##y
#define CONCAT(x, y) CONCAT_(x, y)

#define CHECKTIME(x)                     \
    QElapsedTimer CONCAT(sb_, __LINE__); \
    CONCAT(sb_, __LINE__).start();       \
    x qDebug() << __FUNCTION__ << ":" << __LINE__ << " Elapsed time: " << CONCAT(sb_, __LINE__).elapsed() << " ms."

class BspConfig
{
public:
    enum RadarType
    {
        RADAR_TPYE_OCEAN            = 0x00,
        RADAR_TPYE_LAND             = 0x01,
        RADAR_TPYE_760              = 0x02,
        RADAR_TPYE_DOUBLE_WAVE      = 0x03,
        RADAR_TPYE_DRONE            = 0x04,
        RADAR_TYPE_WATER_GUARD      = 0x05,
        RADAR_TPYE_SECOND_INSTITUDE = 0x06,
    };

    struct Gps_Info
    {
        qint32 week;
        double current_week_ms;
        double latitude;
        double longitude;
        double altitude;
        double roll;     // 翻滚角
        double pitch;    // 俯仰角
        double heading;  // 方位角
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

    // ba必须有4个元素
    static quint32 ba2int(QVector<quint8> ba)
    {
        if(ba.size() != 4)
            return 0;

        quint32 ret = (ba[0] << 24) + (ba[1] << 16) + (ba[2] << 8) + (ba[3] << 0);
        return ret;
    }
};

#endif  // BSP_CONFIG_H
