#include "WaveExtract.h"

bool WaveExtract::isFrameHead(QVector<quint8> frameData, int offset)
{
    return (frameData.at(offset + 0) == 0x01 && frameData.at(offset + 1) == 0x23 && frameData.at(offset + 2) == 0x45 && frameData.at(offset + 3) == 0x67);
}
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
    if(!isFrameHead(frameData, 0))
        return -1;

    WaveformInfo ch;
    int          start_pos = 0;
    int          len       = 0;
    int          offset    = 88;
    ch.motorCnt            = BspConfig::ba2int(frameData.mid(76, 4));

    quint8 skipPointNum = 0;
    while(offset < frameData.size())
    {
        switch(type)
        {
            case BspConfig::RADAR_TPYE_LAND:
                // 第一通道数据
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

                /*
                 * 陆地雷达第二段可能有四个小波形组成，每个小波形又由24个采样点构成
                 * 连续的小波形可能有8个采样点是重复的
                 */
                while(isFrameHead(frameData, offset) == false)  // 说明后面还是波形数据
                {
                    start_pos = (frameData.at(offset) << 24) +
                                (frameData.at(offset + 1) << 16) +
                                (frameData.at(offset + 2) << 8) +
                                (frameData.at(offset + 3) << 0);
                    offset += 4;

                    if(ch.pos.length() != 0 && (ch.pos.last() - 7) == start_pos)  // 两个小波形是连续的
                    {
                        skipPointNum = 8;
                        offset += (skipPointNum * 2);
                    }
                    else
                        skipPointNum = 0;
                    for(int i = 0; i < 24 - skipPointNum; i++)
                    {
                        ch.pos.append(i + skipPointNum + start_pos);
                        ch.value.append((frameData.at(offset + 0) << 8) + frameData.at(offset + 1));
                        offset += 2;
                    }
                    if(offset == frameData.size())  // 数据分析结束
                    {
                        if(ch.pos.length() != 0)
                        {
                            ret.append(ch);  // 数据保存好后，清除当前的缓存，准备重新接收
                        }
                        break;
                    }
                }

                break;
            case BspConfig::RADAR_TPYE_760:
                break;
            case BspConfig::RADAR_TPYE_DRONE:
            case BspConfig::RADAR_TPYE_OCEAN:
            case BspConfig::RADAR_TYPE_WATER_GUARD:
            case BspConfig::RADAR_TPYE_DOUBLE_WAVE:
                if(isChDataHead(frameData, offset))  // 如果这里检查失败，很可能是陆地雷达数据
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
                break;
        }
    }

    //    return ret;
    return 0;
}
