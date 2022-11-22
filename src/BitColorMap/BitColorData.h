#ifndef BITCOLORDATA_H
#define BITCOLORDATA_H

#include <QtCore>
#include <QObject>
#include <QImage>
#include <QPainter>
#include "WaveExtract.h"

/**
 * @brief 使用乒乓操作
 *
 *
 */
class BitColorData : public QObject
{
    Q_OBJECT

public:
    enum MOTOR_CNT_STATE
    {
        IDLE,
        WAIT_START,  // 电机计数重新进入开始范围, 要开始保存数据了
        SAVE_DATA,
        WAIT_END,
    };
    struct SingleSampleData
    {
        QVector<double> data;
        double          angle;
    };

public:
    BitColorData();
    /**
     * @brief
     *
     * @param freq, 外触发频率
     * @param motorSpeed, 电机运动速度 RPM/min
     * @param start_angle,
     * @param end_angle, 如果起始角度和结束角度一致，则说明一圈数据都要保存
     *                   这时需要乒乓操作
     */
    void config(int freq = 5000, int motorSpeed = 1200, double startAngle = 0, double endAngle = 180);

    /**
     * @brief 直接使用上一圈的数据作为基准
     */
    void savingBase(void);

    /**
     * @brief
     *
     * @param allCh, 单次采样的所有通道数据
     */
    void updateData(const QVector<WaveExtract::WaveformInfo> &allCh, int status);

    void generateDiff(QVector<QVector<WaveExtract::WaveformInfo>> &round);

    int  data2rgb(int data, int *r, int *g, int *b);
    void drawLineWithAngle(QImage *img, const QVector<double> &data, double angle);
    void generateImage(void);

signals:
    void bitColorDataReady(QVector<QVector<SingleSampleData>> &result);
    void bitImageReady(QVector<QImage *> image);

private:
    /**
     * @brief
         WaveformInfo, 某个通道某一段的数据
         QVector<WaveExtract::WaveformInfo>, 保存一次采样数据的全部信息.
           如果有4个元素，说明没有第二段采样数据; 如果有8个元素，0，2，4，6为采样数据的第一段；1，3，5，7为采样数据的第二段
         QVector<QVector<WaveExtract::WaveformInfo>> base; 0-180度的全部数据
     */
    uint32_t                                    sampleNumber{0};  // 根据外触发频率，保存相应次数的采样数据作为一圈数据
    QVector<QVector<WaveExtract::WaveformInfo>> base;
    bool                                        tableTenisFlag{false};  // true, 往tableTenis1写数据; false , 往tableTenis2写数据
    QVector<QVector<WaveExtract::WaveformInfo>> tableTennis1;
    QVector<QVector<WaveExtract::WaveformInfo>> tableTennis2;

    /**
     * @brief 数据处理的结果，以通道为单位，一个通道在一圈的采样结果放在一起
     * struct _bit_map_, 一个角度，某个通道的数据
     * QVector<struct _bit_map_>， 全部角度，某个通道的数据
     * QVector<QVector<struct SingleSampleData>>， 4个通道，全部角度，某个通道的数据
     */
    QVector<QVector<SingleSampleData>> result;  // 一圈数据的计算结果

    double   pi{3.1415926};
    uint32_t TICK_PER_CYCLE{163840};

    uint32_t freq{5000};
    uint32_t motorSpeed{0};
    uint32_t startAngle{0};
    uint32_t endAngle{0};
    int      MAX_POINTS{400};
    bool     isSavedBase{false};  // 已经保存好了基准数据

    MOTOR_CNT_STATE state{MOTOR_CNT_STATE::IDLE};

    QVector<QImage *> image;
};
#endif
