#ifndef WAVE_EXTRACT_H
#define WAVE_EXTRACT_H
/*
   接收一次完整的采样数据，分析出波形信息
 */

#include <QtCore>

class WaveExtract : public QObject
{
    Q_OBJECT
private:
    QString radarType;

public:
    enum RadarType {
        RADAR_OCEAN = 0x00,
        RADAR_LAND,
        RADAR_760,
        RADAR_DOUBLE_WAVE,
    };

    // 存放采样数据中，某个具体通道的具体哪一段数据
    struct WaveformInfo
    {
        qint32 number;
        QVector<double> pos;   // 相当于x轴
        QVector<double> value; // 相当于y轴
    };

    WaveExtract() = default;
    int getFrameData(RadarType type, QVector<WaveformInfo> &ret);
};
#endif
