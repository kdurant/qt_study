#ifndef WAVE_DATA_H
#define WAVE_DATA_H
#include <QtCore>
#include <QFile>

/*
 * 分析保存成文件的数据
 */
class ChInfo
{
public:
    qint32 number;
    QVector<double> key;
    QVector<double> value;
};

class OfflineWaveform : public QObject
{
    Q_OBJECT

public:
    OfflineWaveform() : sampleFrameNumber(0) {}
    bool isChDataHead(int offset);
    int getChNumber(int offset);
    void setWaveFile(QString &file);

    QVector<quint8> getFrameData(qint32 number); // 获得一次采样完整的数据
    int getWaveform(QVector<ChInfo> &ret);

public slots:
    qint32 getADsampleNumber();

signals:
    void sendSampleFrameNumber(qint32 number);
    void finishSampleFrameNumber();

private:
    QString         waveFile;
    qint32          sampleFrameNumber;
    QVector<qint32> frameStartPos;
    QVector<quint8> frameData;
};
#endif
