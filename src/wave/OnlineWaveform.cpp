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

    qint32 data_len = ProtocolDispatch::getDataLen(frame);
    if(data_len + FrameField::DATA_POS > frame.size())
    {
        curPckNumber = 0;
        prePckNumber = 0xffff;
        fullSampleWave.clear();
        return;
    }

    curPckNumber = ProtocolDispatch::getPckNum(frame);
    //    number.append(curPckNumber);

    if(curPckNumber == 0)
    {
        if(fullSampleWave.length() != 0)  // A new sampling of data has arrived
        {
            if(isFrameHead(fullSampleWave) != true)
            {
                fullSampleWave.clear();
                fullSampleWave.append(frame.mid(FrameField::DATA_POS, data_len));
                return;
            }
            //#ifdef QT_DEBUG
            //            uint32_t size = fullSampleWave.size();
            //            switch(size)
            //            {
            //                case 284:
            //                case 336:
            //                case 388:
            //                case 440:
            //                case 492:
            //                    break;
            //                default:
            //                    qDebug() << "fullSampleWave.size() = " << fullSampleWave.size();
            //                    break;
            //            }
            //#endif

            emit fullSampleDataReady(fullSampleWave);
            fullSampleWave.clear();
        }
        fullSampleWave.append(frame.mid(FrameField::DATA_POS, data_len));
    }
    else  // 持续接收一次采样数据
    {
        if(fullSampleWave.length() != 0)
            fullSampleWave.append(frame.mid(FrameField::DATA_POS, data_len));
    }
    prePckNumber = curPckNumber;
}
