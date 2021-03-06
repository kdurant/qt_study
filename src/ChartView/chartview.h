#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtCharts>  //必须这么设置

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
    Q_OBJECT
    QLineSeries *ch[8];
    QChart *     charting;

private:
    QPoint beginPoint;  //选择矩形区的起点
    QPoint endPoint;    //选择矩形区的终点
    bool   autoZoomFlag;

public:
    explicit ChartView(QWidget *parent = 0);
    ~ChartView();

    void initChart();
    void setZoomFlag(bool state = false)
    {
        autoZoomFlag = state;
    }

    void updateChart(qint8 chNum, QVector<qint32> &coor, QByteArray &data);

protected:
    void keyPressEvent(QKeyEvent *event);  //按键事件
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

public slots:
    void connectMarkers();
    void disconnectMarkers();
    void handleMarkerClicked();
};

#endif  // CHARTVIEW_H
