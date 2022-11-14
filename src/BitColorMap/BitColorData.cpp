#include "BitColorData.h"

void BitColorData::config(int freq, int motorSpeed, double startAngle, double endAngle)
{
    this->freq       = freq;
    this->motorSpeed = motorSpeed;
    this->startAngle = startAngle;
    this->endAngle   = endAngle;
}

void BitColorData::savingBase(void)
{
    if(tableTenisFlag)
        base = tableTennis2;
    else
        base = tableTennis1;
}

void BitColorData::updateData(const QVector<WaveExtract::WaveformInfo> &allCh)
{
    if(sampleNumber == 0)
    {
        if(tableTenisFlag)
        {
            tableTennis1.clear();
            tableTennis1.resize(freq);
        }
        else
        {
            tableTennis2.clear();
            tableTennis2.resize(freq);
        }
    }

    tableTenisFlag ? tableTennis1.append(allCh) : tableTennis2.append(allCh);

    sampleNumber++;
    if(sampleNumber == static_cast<uint32_t>(freq / (motorSpeed / 60) - 1))
    {
        sampleNumber   = 0;
        tableTenisFlag = !tableTenisFlag;
        if(tableTenisFlag)
            generateDiff(tableTennis2);
        else
            generateDiff(tableTennis1);
    }
}

void BitColorData::generateDiff(QVector<QVector<WaveExtract::WaveformInfo>> &round)
{
    if(round.size() == 0)
        return;
    int base_size  = base.size();
    int round_size = round.size();

    if(qAbs(base_size - round_size) > 20)  // 电机转速应该发生了变化
    {
        return;
    }
    int sample_per_round = std::min(base_size, round_size);

    result.reserve(base[0].size());  // 通道
    for(int i = 0; i < base[0].size(); i++)
        result[i].reserve(sample_per_round);

    QVector<double> data;
    data.reserve(400);
    double angle = 0;

    for(int cycle = 0; cycle < sample_per_round; cycle++)
    {
        QVector<WaveExtract::WaveformInfo> &current  = round[cycle];
        QVector<WaveExtract::WaveformInfo> &previous = base[cycle];

        angle = current[0].motorCnt / TICK_PER_CYCLE;

        for(int ch = 0; ch < previous.size(); ch++)  // 采样数据里的通道
        {
            for(int n = 0, points = std::min(previous[0].value.size(), current[0].value.size()); n < points; n++)
            {
                data.append(current[ch].value[n] - previous[ch].value[n]);
            }
            result[ch][cycle].data  = data;
            result[ch][cycle].angle = angle;
        }
    }
}
