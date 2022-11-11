#include <benchmark/benchmark.h>
#include <algorithm>
#include <QtCore>
#include <QtGui>
// #include <QPolygonF>

#define pi 3.1415926
int data2rgb(int data, int *r, int *g, int *b)
{
    // step 1
    //        double ad = 0;
    //        if(data < 800)
    //            ad = 0.8 * (data / 800);
    //        else
    //            ad = 0.8 + 0.2 * (data - 800) / 3295;

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

void drawLineWithAngle(QImage *img, const QVector<double> &data, double angle)
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

static void bench_drawLineWithAngle(benchmark::State &state)
{
    int             img_size{400};
    QImage         *image = new QImage(img_size, (img_size >> 2) * 3, QImage::Format_RGB32);
    QVector<double> data;
    data.resize(400);
    for(int i = 0; i < 400; i++)
    {
        data[i] = (i + 1) * 2;
    }

    double threshold = state.range(0);
    for(auto _ : state)
    {
        drawLineWithAngle(image, data, 33);
    }
}

BENCHMARK(bench_drawLineWithAngle)
    ->Unit(benchmark::kMicrosecond)
    ->Arg(10000)
    ->Arg(20000);

BENCHMARK_MAIN();
