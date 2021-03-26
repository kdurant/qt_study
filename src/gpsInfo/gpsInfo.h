#ifndef GPSINFO_H
#define GPSINFO_H

#include <QtCore>
#include "bsp_config.h"

/**
 * 目前总共使用过三种GPS
 * 1.
 * 2.
 * 3. APPLANIX， GPS数据帧长115字节
 * 4. 硬盘数据里的GPS信息
 */
class GpsInfo : public QObject
{
    Q_OBJECT

public:
    GpsInfo() = default;
    enum GPS_FRAME_LEN
    {
        APPLANIX_LEN  = 115,
        DISK_DATA_LEN = 88
    };

private:
    BspConfig::Gps_Info gps;

    /**
     * @brief APPLANIX通过串口上传的GPS数据帧
     * @param ppData
     * @return
     */
    double getDouble(unsigned char **ppData)
    {
        double         retValue;
        unsigned char *pBytes;

        pBytes = (unsigned char *)(&retValue) + 7;

        *pBytes-- = *(*ppData)++;
        *pBytes-- = *(*ppData)++;
        *pBytes-- = *(*ppData)++;
        *pBytes-- = *(*ppData)++;
        *pBytes-- = *(*ppData)++;
        *pBytes-- = *(*ppData)++;
        *pBytes-- = *(*ppData)++;
        *pBytes   = *(*ppData)++;

        return retValue;
    }

    /**
     * @brief 硬盘数据中的GPS信息
     * @param bytes
     * @return
     */
    double byteArrayToDouble(QByteArray bytes)
    {
        double  fltRtn = 0.f;
        uint8_t cTmp[8];
        for(int i = 0; i < 8; i++)
            cTmp[i] = bytes[7 - i];
        memcpy(&fltRtn, cTmp, 8);
        return fltRtn;
    }

signals:
    void gpsDataReady(BspConfig::Gps_Info &data);  // 接收到响应数据

public slots:
    void parserGpsData(QByteArray &frame)
    {
        if(frame.size() == APPLANIX_LEN)
        {
            int offset          = 7;
            gps.week            = frame.mid(offset + 2, 2).toHex().toUInt(nullptr, 16);
            gps.current_week_ms = BspConfig::ba2int(frame.mid(offset + 4, 4));
            unsigned char  data[8];
            unsigned char *pData = data;
            for(int i = 0; i < 8; i++)
            {
                pData[i] = frame.mid(offset + 10, 8).at(i);
            }
            gps.latitude = getDouble(&pData);
            for(int i = 0; i < 8; i++)
            {
                pData[i] = frame.mid(offset + 18, 8).at(i);
            }
            gps.longitude = getDouble(&pData);
            for(int i = 0; i < 8; i++)
            {
                pData[i] = frame.mid(offset + 26, 8).at(i);
            }
            gps.altitude = getDouble(&pData);

            for(int i = 0; i < 8; i++)
            {
                pData[i] = frame.mid(offset + 50, 8).at(i);
            }
            gps.roll = getDouble(&pData);
            for(int i = 0; i < 8; i++)
            {
                pData[i] = frame.mid(offset + 58, 8).at(i);
            }
            gps.pitch = getDouble(&pData);
            for(int i = 0; i < 8; i++)
            {
                pData[i] = frame.mid(offset + 66, 8).at(i);
            }
            gps.heading = getDouble(&pData);
        }
        else if(frame.size() == DISK_DATA_LEN)
        {
            gps.week            = frame.mid(8, 4).toHex().toUInt(nullptr, 16);
            gps.current_week_ms = BspConfig::ba2int(frame.mid(12, 8));
            gps.latitude        = byteArrayToDouble(frame.mid(48, 8));
            gps.longitude       = byteArrayToDouble(frame.mid(56, 8));
            gps.altitude        = byteArrayToDouble(frame.mid(64, 8));
            gps.roll            = byteArrayToDouble(frame.mid(40, 8));
            gps.pitch           = byteArrayToDouble(frame.mid(32, 8));
            gps.heading         = byteArrayToDouble(frame.mid(24, 8));
        }
        emit gpsDataReady(gps);
    }
};
;
#endif
