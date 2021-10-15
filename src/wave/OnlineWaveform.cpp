#include "OnlineWaveform.h"

/**
 * @brief OnlineWaveform::getSampleData
 * 开始时，curPckNumber， prePckNumber都是0
 * 工作后，curPckNumber先增加1，数据处理后，prePckNumber增加1
 * 这样当curPckNumber小于prePckNumber的时候，说明接受到一个新的数据帧了
 * @param frame
 */
void OnlineWaveform::getSampleData(QByteArray &frame)
{
    if(!isRecvNewData)
        return;
    curPckNumber = ProtocolDispatch::getPckNum(frame);

    qint32 data_len = ProtocolDispatch::getDataLen(frame);
    //    number.append(curPckNumber);

    if(curPckNumber == 0)
    {
        if(fullSampleWave.length() != 0)
        {
            if(isFrameHead(fullSampleWave) != true)
            {
                fullSampleWave.clear();
                fullSampleWave.append(frame.mid(FrameField::DATA_POS, data_len));
                return;
            }
            emit fullSampleDataReady(fullSampleWave);
            fullSampleWave.clear();
        }
        fullSampleWave.append(frame.mid(FrameField::DATA_POS, data_len));
    }
    else  // 持续接收一次采样数据
    {
        fullSampleWave.append(frame.mid(FrameField::DATA_POS, data_len));
    }

    //    if(curPckNumber < prePckNumber && prePckNumber != 0xffff)  // 已经接收到新一次采集的数据了
    //    {
    //        emit fullSampleDataReady(fullSampleWave);
    //        fullSampleWave.clear();

    //        data_len = ProtocolDispatch::getDataLen(frame);
    //        fullSampleWave.append(frame.mid(FrameField::DATA_POS, data_len));
    //    }
    //    else  // 持续接收一次采样数据
    //    {
    //        data_len = ProtocolDispatch::getDataLen(frame);
    //        fullSampleWave.append(frame.mid(FrameField::DATA_POS, data_len));
    //    }
    prePckNumber = curPckNumber;
}
