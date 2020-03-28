#ifndef PREVIEW_PROCESS_H
#define PREVIEW_PROCESS_H
#include <QtCore>
#include "ProtocolDispatch.h"

typedef struct __OneChData
{
    QVector<qint32> coor;
    QByteArray      data;  // 坐标的2倍长度
} OneChData;

typedef struct __WaveData
{
    OneChData ch[4];
} WaveData;

class PreviewProcess : public QObject
{
    Q_OBJECT
public:
    PreviewProcess()
    {
        pckNumLast   = 0;
        fullFrameCnt = 0;
    }
    WaveData getSignalWave();
    void     setPreviewEnable(int data)
    {
        previewEnable = data;
        emit previewParaReadySet(MasterSet::PREVIEW_ENABLE, 4, data);
    }
    void setPreviewRatio(int data)
    {
        previewRatio = data;
        emit previewParaReadySet(MasterSet::SAMPLE_LEN, 4, data);
    }
    void setTotalSampleLen(int data)
    {
        totalSampleLen = data;
        emit previewParaReadySet(MasterSet::PREVIEW_RATIO, 4, data);
    }
    void setFirstPos(int data)
    {
        firstPos = data;
        emit previewParaReadySet(MasterSet::FIRST_POS, 4, data);
    }
    void setFirstLen(int data)
    {
        firstLen = data;
        emit previewParaReadySet(MasterSet::FIRST_LEN, 4, data);
    }
    void setSecondPos(int data)
    {
        secondPos = data;
        emit previewParaReadySet(MasterSet::SECOND_POS, 4, data);
    }
    void setSecondLen(int data)
    {
        secondLen = data;
        emit previewParaReadySet(MasterSet::SECOND_LEN, 4, data);
    }
    void setCompressLen(int data)
    {
        compressLen = data;
        emit previewParaReadySet(MasterSet::COMPRESS_LEN, 4, data);
    }
    void setCompressRatio(int data)
    {
        compressRatio = data;
        emit previewParaReadySet(MasterSet::COMPRESS_RATIO, 4, data);
    }

public slots:
    void setDataFrame(QByteArray &frame);

signals:
    void previewReadyShow();
    void previewParaReadySet(qint32 command, qint32 data_len, qint32 data);

private:
    int                pckNumLast;
    int                fullFrameCnt;
    QQueue<QByteArray> originFrame;

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
