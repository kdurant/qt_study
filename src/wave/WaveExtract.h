#ifndef WAVE_EXTRACT_H
#define WAVE_EXTRACT_H
/*
   接收一次完整的采样数据，分析出波形信息
 */

#include "bsp_config.h"
#include "common.h"
#include <QtCore>
#include <algorithm>

class WaveExtract : public QObject
{
    Q_OBJECT

public:
    // 存放采样数据中，某个具体通道的具体哪一段数据
    // QVector<WaveformInfo> 来保存一次采样数据的全部信息
    // 如果有4个元素，说明没有第二段采样数据
    // 如果有8个元素，0，2，4，6为采样数据的第一段；1，3，5，7为采样数据的第二段
    struct WaveformInfo
    {
        quint32         motorCnt;     // 电机位置计数
        quint32         maxPosition;  // 波形峰值的位置
        QVector<double> pos;          // 相当于x轴
        QVector<double> value;        // 相当于y轴
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

    void findPeakPosition(QVector<WaveExtract::WaveformInfo> &data);

    friend QDebug operator<<(QDebug debug, const WaveExtract &wave)
    {
        if(wave.ret.size() == 0)
        {
            debug << "没有解析到波形数据";
            return debug;
        }

        QString info = QString("雷达类型: %1  第一段起始位置: %2  第一段长度: %3  第二段起始位置: %4  第二段长度: %5")
                           .arg(wave.radarType)
                           .arg(wave.ret[0].pos[0])
                           .arg(wave.ret[0].pos.size())
                           .arg(wave.ret.size() == 4 ? 0 : wave.ret[1].pos[0])
                           .arg(wave.ret.size() == 4 ? 0 : wave.ret[1].pos.size());
        debug << info;
        return debug;
    }

signals:
    void formatedWaveReady(const QVector<WaveformInfo> &wave, int status);

private:
    BspConfig::RadarType               radarType;
    QVector<WaveExtract::WaveformInfo> ret;
};
#endif
