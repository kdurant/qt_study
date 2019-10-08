#ifndef RADARCHART_H
#define RADARCHART_H

#include <QWidget>
#include <QVBoxLayout>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
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

signals:

public slots:
};

#endif  // RADARCHART_H
