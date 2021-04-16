#ifndef WAVE_EXTRACT_H
#define WAVE_EXTRACT_H
/*
   接收一次完整的采样数据，分析出波形信息
 */

#include "bsp_config.h"
#include <QtCore>

class WaveExtract : public QObject
{
    Q_OBJECT

public:
    // 存放采样数据中，某个具体通道的具体哪一段数据
    struct WaveformInfo
    {
        quint32         motorCnt;  // 电机位置计数
        QVector<double> pos;       // 相当于x轴
        QVector<double> value;     // 相当于y轴
    };

    WaveExtract() = default;

    static bool isFrameHead(QVector<quint8> frameData, int offset);
    static bool isChDataHead(QVector<quint8> frameData, int offset);

    static int getWaveform(BspConfig::RadarType   type,
                           QVector<quint8> &      frameData,
                           QVector<WaveformInfo> &ret);
};
#endif
