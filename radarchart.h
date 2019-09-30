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

    QLineSeries *channal0;
    QChart *chart;
    QChartView *chartView;
    QVBoxLayout *mainLayout;

public:
    explicit RadarChart(QWidget *parent = nullptr);
    void initChart();

signals:

public slots:
};

#endif // RADARCHART_H
