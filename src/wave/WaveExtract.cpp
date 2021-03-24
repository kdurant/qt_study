#include "WaveExtract.h"

/**
 * @brief 判断frameData在offset位置的连续4个数，是否是通道数据的起始标志
 * @param frameData
 * @param offset
 * @return 
 */
bool WaveExtract::isChDataHead(QVector<quint8> frameData, int offset)
{
    if(frameData.size() < offset + 1)
        return false;
    else
        return (frameData.at(offset + 0) == 0xeb && frameData.at(offset + 1) == 0x90 && frameData.at(offset + 2) == 0xa5 && frameData.at(offset + 3) == 0x5a);
}

/**
 * @brief 根据具体的雷达类型，从一次完整的采样数据中，获得具体通道数据波形
 * @param type
 * @param data
 * @param ret
 * @return 
 */
int WaveExtract::getWaveform(BspConfig::RadarType                type,
                             QVector<quint8> &                   frameData,
                             QVector<WaveExtract::WaveformInfo> &ret)
{
    if(frameData[0] != 0x01 || frameData[1] != 0x23 || frameData[2] != 0x45)
        return -1;

    WaveformInfo ch;
    int          start_pos = 0;
    int          len       = 0;
    int          offset    = 88;

    while(offset < frameData.size())
    {
        if(isChDataHead(frameData, offset))
            offset += 4;
        else
            return -1;

        // 第一段数据
        offset += 2;
        start_pos = (frameData.at(offset) << 8) + frameData.at(offset + 1);
        offset += 2;
        len = (frameData.at(offset) << 8) + frameData.at(offset + 1);
        offset += 2;

        for(int i = 0; i < len; i++)
        {
            ch.pos.append(i + start_pos);
            ch.value.append((frameData.at(offset + 0) << 8) + frameData.at(offset + 1));
            offset += 2;
        }
        ret.append(ch);  // 数据保存好后，清除当前的缓存，准备重新接收
        ch.pos.clear();
        ch.value.clear();

        if(offset == frameData.size())  // 数据分析结束
        {
            return 0;
        }
        // 如果第一段数据结束后的数据就是帧头标志，那么说明没有第二段数据
        if(isChDataHead(frameData, offset))
            continue;

        // 第二段数据 offset += 2;
        start_pos = (frameData.at(offset) << 8) + frameData.at(offset + 1);
        offset += 2;
        len = (frameData.at(offset) << 8) + frameData.at(offset + 1);
        offset += 2;

        for(int i = 0; i < len; i++)
        {
            ch.pos.append(i + start_pos);
            ch.value.append((frameData.at(offset + 0) << 8) + frameData.at(offset + 1));
            offset += 2;
        }
        ret.append(ch);  // 数据保存好后，清除当前的缓存，准备重新接收
        ch.pos.clear();
        ch.value.clear();
    }

    //    return ret;
    return -1;
}
int WaveExtract::getWaveform(BspConfig::RadarType type, QByteArray &data, QVector<WaveformInfo> &ret)
{
    return -1;
}
