#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <protocol.h>
#include <QtCharts>  //必须这么设置

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
    Q_OBJECT

    QLineSeries *channal1;
    QLineSeries *channal2;
    QLineSeries *channal3;
    QLineSeries *channal4;
    QChart *     charting;

private:
    QPoint beginPoint;  //选择矩形区的起点
    QPoint endPoint;    //选择矩形区的终点

protected:
    //    void mousePressEvent(QMouseEvent *event);    //鼠标左键按下
    //    void mouseMoveEvent(QMouseEvent *event);     //鼠标移动
    //    void mouseReleaseEvent(QMouseEvent *event);  //鼠标释放左键
    void keyPressEvent(QKeyEvent *event);  //按键事件

public:
    explicit ChartView(QWidget *parent = 0);
    ~ChartView();

    void initChart();
    void updateChart(AD_Data &data);

public slots:
    void connectMarkers();
    void disconnectMarkers();
    void handleMarkerClicked();

signals:
    void mouseMovePoint(QPoint point);  //鼠标移动信号，在mouseMoveEvent()事件中触发
};

#endif  // CHARTVIEW_H
