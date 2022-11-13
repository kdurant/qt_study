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
    int len = std::min(base_size, round_size);
}
