#ifndef BITCOLORDATA_H
#define BITCOLORDATA_H

#include <QtCore>

class BitColorData
{
private:
public:
    BitColorData() = default;
    /**
     * @brief
     *
     * @param freq, 外触发频率
     * @param motorSpeed, 电机运动速度 RPM/min
     * @param start_angle,
     * @param end_angle, 如果起始角度和结束角度一致，则说明一圈数据都要保存
     *                   这时需要乒乓操作
     */
    void config(int freq = 5000, int motorSpeed = 1200, int startAngle, int endAngle);

    /**
     * @brief 在接下来的数据中寻找设定角度范围内的数据作为基准
     *
     * @param mode
     */
    void savingBase(bool mode);

    /**
     * @brief
     *
     * @param allCh, 单次采样的所有通道数据
     */
    void addData(const QVector<WaveExtract::WaveformInfo> &allCh);

private:
    /**
     * @brief
         WaveformInfo, 某个通道某一段的数据
         QVector<WaveExtract::WaveformInfo>, 保存一次采样数据的全部信息.
           如果有4个元素，说明没有第二段采样数据; 如果有8个元素，0，2，4，6为采样数据的第一段；1，3，5，7为采样数据的第二段
         QVector<QVector<WaveExtract::WaveformInfo>> base; 0-180度的全部数据
     */
    QVector<QVector<WaveExtract::WaveformInfo>> base;

    int  freq{0};
    int  motorSpeed{0};
    int  startAngle{0};
    int  endAngle{0};
    bool isSavedBase{false};  // 已经保存好了基准数据
};
#endif
