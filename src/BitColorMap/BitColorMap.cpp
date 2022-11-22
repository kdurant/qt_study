#include "BitColorMap.h"
#include <QPainter>

BitColorMap::BitColorMap(QWidget *parent) :
    QWidget(parent)
{
    resize(img_size, img_size);
    image = new QImage(img_size, img_size, QImage::Format_RGB32);
    image->fill(QColor(62, 62, 62, 150));

    QPainter painter(image);
    painter.setPen(Qt::red);

    painter.drawEllipse(0, 0, std::min(image->width(), image->height()), std::min(image->width(), image->height()));
    //    painter.drawEllipse(0, 0, 200, 200);
    painter.setPen(QPen(Qt::green, 6, Qt::SolidLine, Qt::RoundCap));
    painter.drawPoint(QPoint(image->width() / 2, image->height() / 2));
}

int BitColorMap::data2rgb(int data, int *r, int *g, int *b)
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

/**
 * @brief BitColorMap::setData
 * 使用固定深度的显存。当显存存满数据后，先取出显出对应位置的数据，使用默认颜色，清除显示
 * 然后再显示新的数据
 * @param line, 数据
 * @param angle， 角度
 * @return
 */
int BitColorMap::setData(const QVector<double> &line, double angle)
{
    drawLineWithAngle(image, line, angle);
    return 0;
}

void BitColorMap::drawHalfCircle(QImage *img)
{
    double radius = img->width() / 2;
    double x, y;
    //    for(int i = 180; i >= 0; i--)
    for(int i = 360; i >= 180; i--)
    {
        x = cos((i * pi) / 180);
        y = sin((i * pi) / 180);
        x *= radius;
        y *= radius;
        x += radius;
        y += 1.5 * (img->height() / 2);

        img->setPixelColor(x, y, QColor(Qt::red));
    }
}

/**
 * @brief
 * 数据的长度可能会小于图像的尺寸
 *
 * @param img
 * @param data
 * @param angle
 */
void BitColorMap::drawLineWithAngle(QImage *img, const QVector<double> &data, double angle)
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

void BitColorMap::drawLineColorWithAngle(QImage *img, QColor color, double angle)
{
    double radius = img->width() / 2 - 1;
    double x, y;
    for(int i = 1; i < img->width() / 2; i++)
    {
        x = cos((angle * pi) / 180);
        y = sin((angle * pi) / 180);
        x *= i;
        y *= i;
        x = radius - x;
        y = 1.5 * (img->height() / 2) - y;

        img->setPixelColor(x, y, color);
    }
}

void BitColorMap::paintEvent(QPaintEvent *event)
{
    //    QRect target(0, 0, img_size, img_size);      //建立目标矩形，该区域是显示图像的目的地
    QRect target(0, 0, width(), height());
    QRect source(0.0, 0.0, img_size, img_size);  //建立源矩形，用来划定来自外部的源图像文件中需要显示的区域

    QPainter painter(this);

    painter.drawImage(target, *image, source);
}
