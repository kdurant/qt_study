#include "radarchart.h"

RadarChart::RadarChart(QWidget *parent) : QWidget(parent)
{
    initChart();
    mainLayout = new QVBoxLayout();
    mainLayout->addWidget(chartView);
    setLayout(mainLayout);
}

void RadarChart::initChart()
{
    channal0 = new QLineSeries;
    channal0->append(0, 6);
    channal0->append(2, 4);
    channal0->append(3, 8);
    channal0->append(7, 4);
    channal0->append(10, 5);
    *channal0 << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);


    chart = new QChart();
    chart->addSeries(channal0);
    chart->createDefaultAxes();
    chart->setTitle("雷达实时数据");

    chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setChart(chart);
}
