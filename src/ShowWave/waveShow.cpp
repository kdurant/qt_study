#include "waveShow.h"

void WaveShow::setWaveFile(QString file)
{
    waveFile = file;
}

qint32 WaveShow::getFrameNumber()
{
    QFile file(waveFile);
    file.open(QIODevice::ReadOnly);
    int  length       = 0;
    char buffer[4]    = {0};
    sampleFrameNumber = 0;
    while((length = file.read(buffer, 4)) != 0)
    {
        if(buffer[0] == 0x01 && buffer[1] == 0x23 && buffer[2] == 0x45 && buffer[3] == 0x67)
        {
            sampleFrameNumber++;
            if(sampleFrameNumber % 1000 == 0)
            {
                emit sendSampleFrameNumber(sampleFrameNumber);
                //                ui->lineEdit_validFrameNum->setText(QString::number(sampleFrameNumber));
            }
        }
    }
    emit sendSampleFrameNumber(sampleFrameNumber);
    file.close();
    emit finishSampleFrameNumber();
}
