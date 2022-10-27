#include "WaveExtract.h"

bool WaveExtract::isFrameHead(const QVector<quint8> &frameData, int offset)
{
    return (frameData.at(offset + 0) == 0x01 && frameData.at(offset + 1) == 0x23 && frameData.at(offset + 2) == 0x45 && frameData.at(offset + 3) == 0x67);
}
/**
 * @brief 判断frameData在offset位置的连续4个数，是否是通道数据的起始标志
 * @param frameData
 * @param offset
 * @return
 */

bool WaveExtract::isChDataHead(const QVector<quint8> &frameData, int offset)
{
    assert(frameData.size() > offset + 4);
    return (frameData.at(offset + 0) == 0xeb && frameData.at(offset + 1) == 0x90 && frameData.at(offset + 2) == 0xa5 && frameData.at(offset + 3) == 0x5a);
}

/**
 * @brief 根据具体的雷达类型，从一次完整的采样数据中，获得具体通道数据波形
 * @param type
 * @param data
 * @param ret
 * @return
 */

void WaveExtract::getWaveform(BspConfig::RadarType   type,
                              const QVector<quint8> &frameData)
{
    radarType = type;
    switch(type)
    {
        case BspConfig::RADAR_TYPE_LAND:
            getWaveFromLand(frameData);
            break;
        case BspConfig::RADAR_TYPE_760:
            break;
        case BspConfig::RADAR_TYPE_DRONE:
        case BspConfig::RADAR_TYPE_OCEAN:
        case BspConfig::RADAR_TYPE_WATER_GUARD:
        case BspConfig::RADAR_TYPE_DOUBLE_WAVE:
        case BspConfig::RADAR_TYPE_SECOND_INSTITUDE:
            getWaveFromWaterGuard(frameData);
            break;
    }
}

void WaveExtract::getWaveFromLand(const QVector<quint8> &frameData)
{
    int status = -1;

    if(!isFrameHead(frameData, 0))
        status = -1;
    ret.clear();

    WaveformInfo ch;
    int          start_pos = 0;
    int          len       = 0;
    int          offset    = 88;
    ch.motorCnt            = Common::ba2int(frameData.mid(76, 4), 1);  // BspConfig::ba2int(frameData.mid(76, 4));

    int frame_size = frameData.size();

    quint8 skipPointNum = 0;
    while(offset < frame_size)
    {
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

        if(offset == frame_size)  // 数据分析结束
        {
            status = 0;
            break;
        }

        /*
         * 陆地雷达第二段可能有四个小波形组成，每个小波形又由24个采样点构成
         * 连续的小波形可能有8个采样点是重复的
         */
        // 最多有4个小段波形，每段24个点，相邻的两个小段波形可能会有2个点的数据重叠
        int subWave = 0;
        while(isFrameHead(frameData, offset) == false)  // 说明后面还是波形数据
        {
            start_pos = (frameData.at(offset) << 24) +
                        (frameData.at(offset + 1) << 16) +
                        (frameData.at(offset + 2) << 8) +
                        (frameData.at(offset + 3) << 0);
            offset += 4;

            // 当前小波形和上一个小波形的最后的8个点是重复的， 跳过这8个点不读
            if(ch.pos.length() != 0 && (ch.pos.last() - 7) == start_pos)
            {
                skipPointNum = 8;
                offset += (skipPointNum * 2);
            }
            else
                skipPointNum = 0;

            // 提前判断要取的数据是否超过数组范围
            if(offset + (24 - skipPointNum) *2 > frame_size)
                return;

            for(int i = 0; i < 24 - skipPointNum; i++)
            {
                ch.pos.append(i + skipPointNum + start_pos);
                ch.value.append((frameData.at(offset + 0) << 8) + frameData.at(offset + 1));
                offset += 2;
            }
            subWave++;

            if(offset == frameData.size() || subWave == 4)  // 当数据结束时，保存数据
            {
                if(ch.pos.length() != 0)  // ch里面有了有效数据
                {
                    ret.append(ch);
                }
                break;
            }
        }
    }
    status = 0;
    emit formatedWaveReady(ret, status);
}

/**
 * @brief
 *
 * @param frameData
 * @param ret
 *
 * @return -1, 帧头信息错误; -2, 实际数据长度小于理论值; -3, 通道头信息错误
 */
void WaveExtract::getWaveFromWaterGuard(const QVector<quint8> &frameData)
{
    WaveSettings settings;
    if(getSettingsFromWaterGuard(frameData, settings) == -1)
    {
        emit formatedWaveReady(ret, -1);
        return;
    }
    ret.clear();
    int status     = -1;
    int frame_size = frameData.size();

#if 1
    WaveformInfo ch;
    //    ch.motorCnt = BspConfig::ba2int(frameData.mid(76, 4));
    ch.motorCnt = Common::ba2int(frameData.mid(76, 4), 1);

    int offset = 88;

    while(offset < frame_size)
    {
#if 1
        if(isChDataHead(frameData, offset))
            offset += 4;
        else
        {
            status = -3;
            break;
        }
#endif

        // 第一段数据
        offset += 2;  // 跳过通道号
        offset += 2;  // 跳过第一段起始位置
        offset += 2;  // 跳过第一段长度
#if 1
        for(int i = 0; i < settings.first_len; i++)
        {
            ch.pos.append(i + settings.first_start_pos);
            ch.value.append((frameData.at(offset + 0) << 8) + frameData.at(offset + 1));
            offset += 2;
        }
        ret.append(ch);  // 数据保存好后，清除当前的缓存，准备重新接收
        ch.pos.clear();
        ch.value.clear();

        if(offset == frame_size)  // 数据分析结束
        {
            status = 0;
            break;
        }

        // 如果第一段数据结束后的数据就是帧头标志，那么说明没有第二段数据
        if(isChDataHead(frameData, offset))
            continue;
#endif
#if 1
        // 第二段数据 offset += 2;
        offset += 2;
        offset += 2;

        for(int i = 0; i < settings.second_len; i++)
        {
            ch.pos.append(i + settings.second_start_pos);
            ch.value.append((frameData.at(offset + 0) << 8) + frameData.at(offset + 1));
            offset += 2;
        }
        ret.append(ch);  // 数据保存好后，清除当前的缓存，准备重新接收
        ch.pos.clear();
        ch.value.clear();
#endif
    }
    status = 0;

    findPeakPosition(ret);

    emit formatedWaveReady(ret, status);
#endif
}

/**
 * @brief WaveExtract::getSettingsFromWaterGuard
 * @param frameData
 * @param settings
 * @return 0, 数据完整; -1, 数据不完整
 */
int WaveExtract::getSettingsFromWaterGuard(const QVector<quint8> &frameData, WaveExtract::WaveSettings &settings)
{
    int frame_size = frameData.size();
    if(frame_size < 88)
        return -1;
    if(!isFrameHead(frameData, 0))
        return -1;

    int index = 88;
    if(isChDataHead(frameData, index) == false)
        return -1;

    index += 6;
    settings.first_start_pos = (frameData.at(index) << 8) + frameData.at(index + 1);
    index += 2;
    settings.first_len = (frameData.at(index) << 8) + frameData.at(index + 1);
    index += 2;
    index += settings.first_len * 2;

    if(frame_size < index + 4)  // 数据已经丢失，还去计算第二段信息，会导致ASSERT failure in QVector<T>::at: "index out of range"
        return -1;

    if(isChDataHead(frameData, index))
    {
        settings.second_start_pos = 0;
        settings.second_len       = 0;
    }
    else
    {
        settings.second_start_pos = (frameData.at(index) << 8) + frameData.at(index + 1);
        index += 2;
        settings.second_len = (frameData.at(index) << 8) + frameData.at(index + 1);
    }

    /*
    水下预警雷达：
        设第一段采样数据长度:m, 则总共采样长度:L
        L = 128  + 8 * m
        第二段采样长度: n
        4*(n*2 + 4)
    */

    int expect_len = 0;
    expect_len += 128 + 8 * settings.first_len;
    if(settings.second_len > 0)
        expect_len += 4 * (settings.second_len * 2 + 4);

    // 全部长度小于4个通道第一段数据的总和
    if(frame_size != expect_len)
        return -1;

    return 0;
}

void WaveExtract::findPeakPosition(QVector<WaveformInfo> &data)
{
    int len = data.size();

    auto peakPosition = [&](int i)
    {
        auto max            = std::max_element(std::begin(data[i].pos), std::end(data[i].pos));
        data[i].maxPosition = std::distance(std::begin(data[i].pos), max);
    };

    for(int i = 0; i < len; i++)
    {
        peakPosition(i);
    }
}
