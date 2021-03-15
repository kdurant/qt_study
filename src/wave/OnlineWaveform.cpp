#include "OnlineWaveform.h"

/**
 * @brief OnlineWaveform::getSampleData
 * 1. 开始时，curPckNumber， prePckNumber都是0
 * 2. 工作后，curPckNumber先增加1，
 * 3. 数据处理后，prePckNumber增加1
 * 4.
 * @param frame
 */
void OnlineWaveform::getSampleData(QByteArray &frame)
{
    qint32 data_len = 0;
    if(!isRecvNewData)
        return;
    curPckNumber = ProtocolDispatch::getPckNum(frame);

    if(curPckNumber < prePckNumber)  // 已经接收到新一次采集的数据了
    {
        emit fullSampleDataReady(fullSampleWave);
        fullSampleWave.clear();

        data_len = ProtocolDispatch::getDataLen(frame);
        fullSampleWave.append(frame.mid(FrameField::DATA_POS, data_len));
    }
    else
    {
        data_len = ProtocolDispatch::getDataLen(frame);
        fullSampleWave.append(frame.mid(FrameField::DATA_POS, data_len));
    }
    prePckNumber = curPckNumber;
}
