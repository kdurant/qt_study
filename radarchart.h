#ifndef RADARCHART_H
#define RADARCHART_H

#include <QWidget>
#include <QObject>
#include <QVBoxLayout>
#include <QGraphicsWidget>
#include <QDebug>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QChartGlobal>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QXYLegendMarker>

QT_CHARTS_USE_NAMESPACE

class RadarChart : public QWidget
{
    Q_OBJECT

    QLineSeries *channal1;
    QLineSeries *channal2;
    QLineSeries *channal3;
    QLineSeries *channal4;
    QChart *     chart;
    QChartView * chartView;
    QVBoxLayout *mainLayout;

public:
    explicit RadarChart(QWidget *parent = nullptr);
    void initChart();
    void updateChart();

public slots:
    void connectMarkers();
    void disconnectMarkers();
    void handleMarkerClicked();

signals:

public slots:
};

#endif  // RADARCHART_H
