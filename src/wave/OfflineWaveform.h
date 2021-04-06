#ifndef WAVE_DATA_H
#define WAVE_DATA_H
#include <QtCore>
#include <QFile>

/*
 * 分析保存成文件的数据
 */

class OfflineWaveform : public QObject
{
    Q_OBJECT

public:
    OfflineWaveform() :
        sampleFrameNumber(0)
    {
    }
    void setWaveFile(QString &file);

    QVector<quint8> getFrameData(qint32 number);  // 获得一次采样完整的数据
    quint32         getMotorCnt(qint32 number);

public slots:
    qint32 getADsampleNumber();

signals:
    void sendSampleFrameNumber(qint32 number);  // 用于表示分析大文件的实时进度
    void finishSampleFrameNumber();             // 大文件分析完成，通知线程退出

private:
    QString          waveFile;
    qint32           sampleFrameNumber;
    QVector<quint32> frameStartPos;  // 单次采样数据开始在文件中的位置
    QVector<quint8>  frameData;
};
#endif
