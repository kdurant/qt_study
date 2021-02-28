#include "OnlineWaveform.h"

void OnlineWaveform::getSampleData(QByteArray &frame)
{
    if (!isRecvNewData)
        return;
    curPckNumber = ProtocolDispatch::getPckNum(frame);

    if (curPckNumber < prePckNumber) // 已经接收到新一次采集的数据了
    {
        emit fullSampleDataReady(fullSampleWave);
        fullSampleWave.clear();
    } else {
        qint32 data_len = ProtocolDispatch::getDataLen(frame);
        fullSampleWave.append(frame.mid(24, data_len));
    }
    prePckNumber = curPckNumber;
}
