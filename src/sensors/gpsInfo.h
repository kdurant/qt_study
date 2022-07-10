#ifndef GPSINFO_H
#define GPSINFO_H

#include <QtCore>
#include "bsp_config.h"
#include "common.h"

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

    // 不同型号GPS上传的数据内容不一致，长度也不一致
    // 所以可以根据上传数据的长度来判断GPS设备类型，进而解析数据
    enum GPS_FRAME_LEN
    {
        APPLANIX_LEN     = 115,  // 第一个字节0x02
        NOVATEL_LEN      = 115,  // 前3个字节数据0xAA,0x44,0x13
        DISK_DATA_LEN    = 88,   // 硬盘数据里包括帧头的GPS数据
        PREVIEW_DATA_LEN = 64,   // 预览数据里的GPS数据
    };

private:
    BspConfig::Gps_Info gps{0, 0, 0, 0, 0, 0, 0, 0, 0};

    /**
     * @brief APPLANIX通过串口上传的GPS数据帧
     * @param ppData
     * @return
     */
    double getDouble(unsigned char *ppData)
    {
        double         retValue;
        unsigned char *pBytes;

        pBytes = (unsigned char *)(&retValue) + 7;

        for(int i = 0; i < 8; i++)
            *(pBytes - i) = *(ppData + i);

        return retValue;
    }
    void paserGpsData_APPLANIX(QByteArray &frame)
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
        gps.latitude = getDouble(pData);
        for(int i = 0; i < 8; i++)
        {
            pData[i] = frame.mid(offset + 18, 8).at(i);
        }
        gps.longitude = getDouble(pData);
        for(int i = 0; i < 8; i++)
        {
            pData[i] = frame.mid(offset + 26, 8).at(i);
        }
        gps.altitude = getDouble(pData);

        for(int i = 0; i < 8; i++)
        {
            pData[i] = frame.mid(offset + 50, 8).at(i);
        }
        gps.roll = getDouble(pData);
        for(int i = 0; i < 8; i++)
        {
            pData[i] = frame.mid(offset + 58, 8).at(i);
        }
        gps.pitch = getDouble(pData);
        for(int i = 0; i < 8; i++)
        {
            pData[i] = frame.mid(offset + 66, 8).at(i);
        }
        gps.heading = getDouble(pData);
    }

    void paserGpsData_NOVATEL(QByteArray &frame)
    {
        int offset = 12;
        gps.week   = Common::ba2int(frame.mid(offset, 4), 0);
        // TODO: 不同GPS型号对秒的存储方式不同，所以解析的类型的方式不同，后续完善
        gps.current_week_ms = Common::byteArrayToDouble(frame.mid(offset + 4, 8), 1);
        gps.latitude        = Common::byteArrayToDouble(frame.mid(offset + 12, 8), 1);
        gps.longitude       = Common::byteArrayToDouble(frame.mid(offset + 20, 8), 1);
        gps.altitude        = Common::byteArrayToDouble(frame.mid(offset + 28, 8), 1);
        gps.height          = gps.altitude;

        gps.roll    = Common::byteArrayToDouble(frame.mid(offset + 60, 8), 1);
        gps.pitch   = Common::byteArrayToDouble(frame.mid(offset + 68, 8), 1);
        gps.heading = Common::byteArrayToDouble(frame.mid(offset + 76, 8), 1);
    }
signals:
    void gpsDataReady(BspConfig::Gps_Info &data);  // 接收到响应数据

public slots:
    void parserGpsData(QByteArray &frame)
    {
        if(frame.size() == APPLANIX_LEN)
        {
            if(frame.front() == char(0x02))
                paserGpsData_APPLANIX(frame);
            else
                paserGpsData_NOVATEL(frame);
        }
        else if(frame.size() == DISK_DATA_LEN)
        {
            gps.week = frame.mid(8, 4).toHex().toUInt(nullptr, 16);
            // TODO: 不同GPS型号的秒都是占用8个字节，但数据类型不同，所以解析方式不同，后续完善
            // NOVATEL GPS 秒存储方式：8字节，按照double类型
            // XW-GI7660 GPS 秒存储方式：高4字节0，低4字节按照uint32类型处理, 比例系数0.001
            // APPLANIX GPS 秒存储方式：高4字节0，低4字节按照uint32类型处理，单位ms
            //            gps.current_week_ms = BspConfig::ba2int(frame.mid(12, 8));
            if(frame.at(12) == 0 && frame.at(13) == 0)
                gps.current_week_ms = Common::ba2int(frame.mid(16, 4), 1) / 1000;
            else
                gps.current_week_ms = Common::byteArrayToDouble(frame.mid(12, 8), 0);  // 无人机雷达GPS格式
            gps.sub_time  = Common::ba2int(frame.mid(20, 4), 1);
            gps.latitude  = Common::byteArrayToDouble(frame.mid(48, 8), 0);
            gps.longitude = Common::byteArrayToDouble(frame.mid(56, 8), 0);
            gps.altitude  = Common::byteArrayToDouble(frame.mid(64, 8), 0);
            gps.roll      = Common::byteArrayToDouble(frame.mid(40, 8), 0);
            gps.pitch     = Common::byteArrayToDouble(frame.mid(32, 8), 0);
            gps.heading   = Common::byteArrayToDouble(frame.mid(24, 8), 0);
        }
        emit gpsDataReady(gps);
    }
};
;
#endif