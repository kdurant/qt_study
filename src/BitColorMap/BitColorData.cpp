#include "BitColorData.h"

BitColorData::BitColorData()
{
}

void BitColorData::config(int freq, int motorSpeed, double startAngle, double endAngle)
{
    this->freq       = freq;
    this->motorSpeed = motorSpeed;
    this->startAngle = startAngle;
    this->endAngle   = endAngle;
}

void BitColorData::savingBase(void)
{
    if(tableTenisFlag)
        base = tableTennis2;
    else
        base = tableTennis1;
}

void BitColorData::updateData(const QVector<WaveExtract::WaveformInfo> &allCh, int status)
{
    qDebug() << "BitColorData::updateData() run in Thread = " << QThread::currentThreadId();
    QVector<WaveExtract::WaveformInfo> data;

    if(allCh.size() == 8)
    {
        data.reserve(4);
        data.append(allCh[1]);
        data.append(allCh[3]);
        data.append(allCh[5]);
        data.append(allCh[7]);
    }
    else
        data = allCh;

    if(sampleNumber == 0)
    {
        if(tableTenisFlag)
        {
            tableTennis1.clear();
            tableTennis1.reserve(freq);
        }
        else
        {
            tableTennis2.clear();
            tableTennis2.reserve(freq);
        }
    }

    tableTenisFlag ? tableTennis1.append(data) : tableTennis2.append(data);

    sampleNumber++;
    // 保存到一圈数据后，和基准数据比较
    if(sampleNumber == static_cast<uint32_t>(freq / (motorSpeed / 60) - 1))
    {
        sampleNumber   = 0;
        tableTenisFlag = !tableTenisFlag;
        if(tableTenisFlag)
            generateDiff(tableTennis2);
        else
            generateDiff(tableTennis1);
        generateImage();
    }
}

void BitColorData::generateDiff(QVector<QVector<WaveExtract::WaveformInfo>> &round)
{
    qDebug() << "BitColorData::generateDiff() run in Thread = " << QThread::currentThreadId();
    if(round.size() == 0)
        return;
    int    base_size  = base.size();
    int    round_size = round.size();  // 一圈里有多少次采样数据
    double angle      = 0;

    QVector<double> data;

    if(base_size == 0)  // 没有保存基准数据时
    {
        result.resize(round[0].size());  // 通道
        for(int i = 0; i < round[0].size(); i++)
            result[i].resize(round_size);

        for(int cycle = 0; cycle < round_size; cycle++)
        {
            QVector<WaveExtract::WaveformInfo> &current = round[cycle];

            angle = current[0].motorCnt / TICK_PER_CYCLE;

            for(int ch = 0; ch < current.size(); ch++)  // 采样数据里的通道
            {
                data.clear();
                data.reserve(MAX_POINTS);
                for(int n = 0, points = current[0].value.size(); n < std::min(MAX_POINTS, points); n++)
                {
                    data.append(current[ch].value[n]);
                }
                result[ch][cycle].data  = data;
                result[ch][cycle].angle = angle;
            }
        }
        //        emit bitColorDataReady(result);
        return;
    }

    if(qAbs(base_size - round_size) > 20)  // 电机转速应该发生了变化
    {
        return;
    }

    int sample_per_round = std::min(base_size, round_size);

    result.resize(base[0].size());  // 通道
    for(int i = 0; i < base[0].size(); i++)
        result[i].resize(sample_per_round);

    for(int cycle = 0; cycle < sample_per_round; cycle++)
    {
        QVector<WaveExtract::WaveformInfo> &current  = round[cycle];
        QVector<WaveExtract::WaveformInfo> &previous = base[cycle];

        angle = current[0].motorCnt / TICK_PER_CYCLE;

        for(int ch = 0; ch < previous.size(); ch++)  // 采样数据里的通道
        {
            data.clear();
            data.reserve(MAX_POINTS);
            for(int n = 0, points = std::min(previous[0].value.size(), current[0].value.size()); n < points; n++)
            {
                data.append(current[ch].value[n] - previous[ch].value[n]);
            }
            result[ch][cycle].data  = data;
            result[ch][cycle].angle = angle;
        }
    }
    //    emit bitColorDataReady(result);
}

int BitColorData::data2rgb(int data, int *r, int *g, int *b)
{
    int f = ((data - 0.0) / (1024 - 0)) * 255;

    if(f >= 0 && f <= 63)
    {
        *r = 0;
        *g = 254 - 4 * f;
        *b = 255;
    }
    else if(f >= 64 && f <= 127)
    {
        *r = 0;
        *g = 4 * f - 254;
        *b = 510 - 4 * f;
    }
    else if(f >= 128 && f <= 191)
    {
        *r = 4 * f - 510;
        *g = 255;
        *b = 0;
    }
    else if(f >= 192 && f <= 255)
    {
        *r = 255;
        *g = 1022 - 4 * f;
        *b = 0;
    }
    return 0;
}

void BitColorData::drawLineWithAngle(QImage *img, const QVector<double> &data, double angle)
{
    double radius = img->width() / 2 - 1;
    double x, y;
    int    r = 0;
    int    g = 0;
    int    b = 0;

    int len = data.size();

    for(int i = 1; i < img->width() / 2 && i < len / 2; i++)
    // for(int i = 1; i < img->width() / 2; i++)
    {
        x = qCos((angle * pi) / 180);
        y = qSin((angle * pi) / 180);
        x *= i;
        y *= i;
        x = radius - x;
        y = 1.5 * (img->height() / 2) - y;

        data2rgb(data[i * 2], &r, &g, &b);  // 间隔取点，不然显示不了
        img->setPixelColor(x, y, QColor(r, g, b));
    }
}

void BitColorData::generateImage()
{
    image.resize(4);
    for(int i = 0; i < image.size(); i++)
    {
        image[i] = new QImage(MAX_POINTS, (MAX_POINTS >> 2) * 3, QImage::Format_RGB32);
        for(int i = 0; i < image[i]->width(); i += 1)  // change background color
        {
            for(int j = 0; j < image[i]->height(); ++j)
            {
                image[i]->setPixelColor(i, j, QColor(62, 62, 62));
            }
        }
        //        image[i]->fill(QColor(62, 62, 62));

        // 在图像上画个半圆
        double radius = image[i]->width() / 2;
        double x, y;
        //    for(int i = 180; i >= 0; i--)
        for(int i = 360; i >= 180; i--)
        {
            x = cos((i * pi) / 180);
            y = sin((i * pi) / 180);
            x *= radius;
            y *= radius;
            x += radius;
            y += 1.5 * (image[i]->height() / 2);

            image[i]->setPixelColor(x, y, QColor(Qt::red));
        }
    }

    for(int i = 0; i < result.size(); i++)
    {
        for(int j = 0; j < result[0].size(); j++)
        {
            drawLineWithAngle(image[i], result[i][j].data, result[i][j].angle);
        }
    }
}
