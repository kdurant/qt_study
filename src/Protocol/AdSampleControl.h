#ifndef PREVIEW_PROCESS_H
#define PREVIEW_PROCESS_H
#include <QtCore>
#include "protocol.h"
#include "ProtocolDispatch.h"

/*
 * 设置AD采样参数
 */
class AdSampleControll : public QObject
{
    Q_OBJECT
public:
    AdSampleControll() {}
    void setPreviewEnable(int data)
    {
        previewEnable = data;
        emit AdParaDataReady(MasterSet::PREVIEW_ENABLE, 4, data);
    }
    void setTotalSampleLen(int data)
    {
        totalSampleLen = data;
        emit AdParaDataReady(MasterSet::SAMPLE_LEN, 4, data);
    }
    void setPreviewRatio(int data)
    {
        previewRatio = data;
        emit AdParaDataReady(MasterSet::PREVIEW_RATIO, 4, data);
    }
    void setFirstPos(int data)
    {
        firstPos = data;
        emit AdParaDataReady(MasterSet::FIRST_POS, 4, data);
    }
    void setFirstLen(int data)
    {
        firstLen = data;
        emit AdParaDataReady(MasterSet::FIRST_LEN, 4, data);
    }
    void setSecondPos(int data)
    {
        secondPos = data;
        emit AdParaDataReady(MasterSet::SECOND_POS, 4, data);
    }
    void setSecondLen(int data)
    {
        secondLen = data;
        emit AdParaDataReady(MasterSet::SECOND_LEN, 4, data);
    }
    void setCompressLen(int data)
    {
        compressLen = data;
        emit AdParaDataReady(MasterSet::COMPRESS_LEN, 4, data);
    }
    void setCompressRatio(int data)
    {
        compressRatio = data;
        emit AdParaDataReady(MasterSet::COMPRESS_RATIO, 4, data);
    }

signals:
    void AdParaDataReady(qint32 command, qint32 data_len, qint32 data);

private:
    int previewEnable;

    int totalSampleLen;
    int previewRatio;
    int firstPos;
    int firstLen;
    int secondPos;
    int secondLen;
    int compressLen;
    int compressRatio;
};
#endif
