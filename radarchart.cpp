#include "radarchart.h"

QT_CHARTS_USE_NAMESPACE

RadarChart::RadarChart(QWidget *parent)
    : QWidget(parent)
{
    initChart();
    mainLayout = new QVBoxLayout();
    mainLayout->addWidget(chartView);
    setLayout(mainLayout);

    connectMarkers();
}

void RadarChart::initChart()
{
    channal1 = new QLineSeries;
    channal1->setName("channal0");
    channal1->append(1, 1);
    channal1->append(2, 2);
    channal1->append(3, 3);
    channal1->append(4, 4);
    channal1->append(5, 0);

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

    chart->axisX()->setRange(0, 300);
    chart->axisY()->setRange(0, 1000);

    chart->setTitle("雷达实时数据");
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QChartView::RectangleRubberBand);
}

void RadarChart::updateChart(AD_Data &data)
{
    qint32 x = 0, y = 0;
    channal1->clear();

    //    *channal1 << QPointF(1, 5) << QPointF(2, 4) << QPointF(3, 3) << QPointF(4, 2) << QPointF(5, 1);

    x = data.first_start_pos;
    for(int i = 0; i < data.first_data.size(); i += 4)
    {
        y = data.first_data.mid(i, 4).toInt(nullptr, 16);
        channal1->append(x, y);
        x++;
    }
}

void RadarChart::connectMarkers()
{
    //![1]
    // Connect all markers to handler
    foreach(QLegendMarker *marker, chart->legend()->markers())
    {
        // Disconnect possible existing connection to avoid multiple connections
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
        QObject::connect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
    //![1]
}

void RadarChart::disconnectMarkers()
{
    //![2]
    foreach(QLegendMarker *marker, chart->legend()->markers())
    {
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
    //![2]
}

void RadarChart::handleMarkerClicked()
{
    //![3]
    QLegendMarker *marker = qobject_cast<QLegendMarker *>(sender());
    Q_ASSERT(marker);
    //![3]

    //![4]
    switch(marker->type())
    //![4]
    {
        case QLegendMarker::LegendMarkerTypeXY:
        {
            //![5]
            // Toggle visibility of series
            marker->series()->setVisible(!marker->series()->isVisible());

            // Turn legend marker back to visible, since hiding series also hides the marker
            // and we don't want it to happen now.
            marker->setVisible(true);
            //![5]

            //![6]
            // Dim the marker, if series is not visible
            qreal alpha = 1.0;

            if(!marker->series()->isVisible())
            {
                alpha = 0.5;
            }

            QColor color;
            QBrush brush = marker->labelBrush();
            color        = brush.color();
            color.setAlphaF(alpha);
            brush.setColor(color);
            marker->setLabelBrush(brush);

            brush = marker->brush();
            color = brush.color();
            color.setAlphaF(alpha);
            brush.setColor(color);
            marker->setBrush(brush);

            QPen pen = marker->pen();
            color    = pen.color();
            color.setAlphaF(alpha);
            pen.setColor(color);
            marker->setPen(pen);

            //![6]
            break;
        }
        default:
        {
            qDebug() << "Unknown marker type";
            break;
        }
    }
}

void RadarChart::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Plus:
            chartView->chart()->zoomIn();
            break;
        case Qt::Key_Minus:
            chartView->chart()->zoomOut();
            break;
            //![1]
        case Qt::Key_Left:
            chartView->chart()->scroll(-10, 0);
            break;
        case Qt::Key_Right:
            chartView->chart()->scroll(10, 0);
            break;
        case Qt::Key_Up:
            chartView->chart()->scroll(0, 10);
            break;
        case Qt::Key_Down:
            chartView->chart()->scroll(0, -10);
            break;
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}
