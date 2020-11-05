#ifndef WAVE_DATA_H
#define WAVE_DATA_H
#include <QtCore>

class WaveData : public QObject
{
    Q_OBJECT

public:
    WaveData()
    {
    }

private:
    QVector<int> ch0;
};
#endif
