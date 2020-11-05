#ifndef WAVE_DATA_H
#define WAVE_DATA_H
#include <QtCore>
#include <QFile>

class WaveShow : public QObject
{
    Q_OBJECT

public:
    WaveShow()
        : sampleFrameNumber(0)
    {
    }
    void setWaveFile(QString file);
    QVector<qint32> getFrameData(qint32 number);

public slots:
    qint32 getFrameNumber();

signals:
    void sendSampleFrameNumber(qint32 number);
    void finishSampleFrameNumber();

private:
    QString         waveFile;
    qint32          sampleFrameNumber;
    QVector<qint32> frameStartPos;
};
#endif
