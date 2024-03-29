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

    double getDistance(void)
    {
        return distance;
    }

    /**
     * @brief 4个通道，每个通道有两段波形
     * 第一通道的一段数据中(data[0])，找出主波峰值点
     * data[1], data[3], data[5], data[7]中分别找出每个通道第二段数据的峰值点
     * 最后计算峰值点的距离
     * @param data
     */
    static double calcDistance(QVector<WaveExtract::WaveformInfo> &data)
    {
        int len = data.size();
        if(len != 8)
            return -1;

        /**
         * 从value中找出最大值，判断这个最大值的位置，再加上pos[0]的数字
         * 即是峰值点在采样序列中的时间(ns)
         */
        auto peakPosition = [&](int i)
        {
            auto max            = std::max_element(std::begin(data[i].value), std::end(data[i].value));
            data[i].maxPosition = std::distance(std::begin(data[i].value), max) + data[i].pos[0];
        };

        peakPosition(0);
        peakPosition(1);
        peakPosition(3);
        peakPosition(5);
        peakPosition(7);
        int main_start = data[0].maxPosition;
        int ch0_result = data[1].maxPosition;
        int ch1_result = data[3].maxPosition;
        int ch2_result = data[5].maxPosition;
        int ch3_result = data[7].maxPosition;

        return (ch1_result - main_start) * 0.15;
    }

    static double calcAngle(QVector<WaveExtract::WaveformInfo> &data, int motorMaxValue)
    {
        return static_cast<double>(data[0].motorCnt * 360) / motorMaxValue;
    }

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
    /**
     * @brief formatedWaveReady
     * @param wave, 单次采样中所有通道的数据，最多有4*2 = 8
     * @param status
     */
    void formatedWaveReady(const QVector<WaveformInfo> &wave, int status);

private:
    BspConfig::RadarType               radarType;
    QVector<WaveExtract::WaveformInfo> ret;
    double                             distance{0};
};
#endif
