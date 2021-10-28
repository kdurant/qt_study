#ifndef WAVE_EXTRACT_H
#define WAVE_EXTRACT_H
/*
   接收一次完整的采样数据，分析出波形信息
 */

#include "bsp_config.h"
#include "common.h"
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

    struct WaveSettings
    {
        int first_start_pos;
        int first_len;
        int second_start_pos;
        int second_len;
    };

    enum MOTOR_CNT_STATE
    {
        IDLE,
        WAIT_START,  // 电机计数重新进入开始范围, 要开始保存数据了
        LOAD_DATA,
        WAIT_END,
    };

    WaveExtract() = default;

    static bool isFrameHead(const QVector<quint8> &frameData, int offset);
    static bool isChDataHead(const QVector<quint8> &frameData, int offset);

    void getWaveform(BspConfig::RadarType type, const QVector<quint8> &frameData);
    void getWaveFromLand(const QVector<quint8> &frameData);
    void getWaveFromWaterGuard(const QVector<quint8> &frameData);
    int  getSettingsFromWaterGuard(const QVector<quint8> &frameData, WaveSettings &settings);

signals:
    void formatedWaveReady(const QVector<WaveformInfo> &wave, int status);

private:
    BspConfig::RadarType radarType;
};
#endif
