#include "radarchart.h"

RadarChart::RadarChart(QWidget *parent)
    : QWidget(parent)
{
    initChart();
    mainLayout = new QVBoxLayout();
    mainLayout->addWidget(chartView);
    setLayout(mainLayout);
}

void RadarChart::initChart()
{
    channal1 = new QLineSeries;
    channal1->setName("channal0");
    channal1->append(0, 6);
    channal1->append(2, 4);
    channal1->append(3, 8);
    channal1->append(7, 4);
    channal1->append(10, 5);
    *channal1 << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

    channal2 = new QLineSeries;
    channal2->setName("channal2");
    channal3 = new QLineSeries;
    channal3->setName("channal3");
    channal4 = new QLineSeries;
    channal4->setName("channal4");

    chart = new QChart();
    chart->addSeries(channal1);
    chart->addSeries(channal2);
    chart->addSeries(channal3);
    chart->addSeries(channal4);
    chart->createDefaultAxes();
    chart->setTitle("雷达实时数据");

    chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setChart(chart);
}
