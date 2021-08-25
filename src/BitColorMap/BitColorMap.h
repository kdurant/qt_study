#ifndef COLOR_H
#define COLOR_H

#include <QtCore>
#include <QWidget>
#include <QPainter>

class BitColorMap : public QWidget
{
    Q_OBJECT
public:
    explicit BitColorMap(QWidget *parent);

    struct LaneData
    {
        QVector<double> data;
        double          angle;
    };

    int data2rgb(int data, int *r, int *g, int *b);
    int setData(QVector<double> &line, double angle);

    void drawHalfCircle(QImage *img);
    void drawLineWithAngle(QImage *img, QVector<double> &data, double angle);

    // 用于清除已经画了的线
    void drawLineColorWithAngle(QImage *img, QColor color, double angle);

    void sleepWithoutBlock(qint32 interval)
    {
        QEventLoop waitLoop;
        QTimer::singleShot(interval, &waitLoop, &QEventLoop::quit);
        waitLoop.exec();
    }
    void setVideoMemoryDepth(int d)
    {
        depth = d;
        videoMemory.resize(depth);
    }

protected:
    //    void resizeEvent(QResizeEvent *event);  //大小重置事件
    void paintEvent(QPaintEvent *event);

private:
    int     img_size;
    double  pi{3.1415926};
    QImage *image;
    int64_t count{0};  // 统计总共接收到了多少次数据

    int               depth;  // 队列里缓存多少次采样数据
    QVector<LaneData> videoMemory;

signals:
};

#endif  // COLOR_H
