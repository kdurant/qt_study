#include <benchmark/benchmark.h>
#include <algorithm>
#include <QtCore>
#include <QPolygonF>

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
    int             image_size{400};
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

    BENCHMARK(bench_drawLineWithAngle)
        ->Unit(benchmark::kMillisecond)
        ->Arg(5)
        ->Arg(10)
        ->Arg(20);
}

BENCHMARK_MAIN();
