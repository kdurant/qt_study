#include "chartview.h"
#include <QChartView>

QT_CHARTS_USE_NAMESPACE

ChartView::ChartView(QWidget *parent)
    : QChartView(parent)
{
    this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setMouseTracking(true);  //必须开启此功能
    //    this->setRubberBand(QChartView::RectangleRubberBand);//设置为矩形选择方式
    //    this->setRubberBand(QChartView::VerticalRubberBand);
    //    this->setRubberBand(QChartView::HorizontalRubberBand);

    initChart();
    connectMarkers();
}

ChartView::~ChartView()
{
    // 必须保留此函数
}

void ChartView::keyPressEvent(QKeyEvent *event)
{  //按键控制
    switch(event->key())
    {
        case Qt::Key_Plus:  //+
            this->chart()->zoom(1.2);
            break;
        case Qt::Key_Minus:
            this->chart()->zoom(0.8);
            break;
        case Qt::Key_Left:
            this->chart()->scroll(10, 0);
            break;
        case Qt::Key_Right:
            this->chart()->scroll(-10, 0);
            break;
        case Qt::Key_Up:
            this->chart()->scroll(0, -10);
            break;
        case Qt::Key_Down:
            this->chart()->scroll(0, 10);
            break;
        case Qt::Key_PageUp:
            this->chart()->scroll(0, 50);
            break;
        case Qt::Key_PageDown:
            this->chart()->scroll(0, -50);
            break;
        case Qt::Key_Home:
            this->chart()->zoomReset();
            break;
        default:
            QGraphicsView::keyPressEvent(event);
    }
    //    QGraphicsView::keyPressEvent(event);
}

void ChartView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        this->chart()->zoomReset();
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    qreal factor;
    if(event->delta() > 0)
        factor = 2.0;
    else
        factor = 0.5;

    QRectF  r        = QRectF(chart()->plotArea().left(), chart()->plotArea().top(),
                      chart()->plotArea().width() / factor, chart()->plotArea().height() / factor);
    QPointF mousePos = mapFromGlobal(QCursor::pos());
    r.moveCenter(mousePos);
    chart()->zoomIn(r);
    QPointF delta = chart()->plotArea().center() - mousePos;
    chart()->scroll(delta.x(), -delta.y());

    QChartView::wheelEvent(event);
}

void ChartView::connectMarkers()
{
    // Connect all markers to handler
    foreach(QLegendMarker *marker, charting->legend()->markers())
    {
        // Disconnect possible existing connection to avoid multiple connections
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
        QObject::connect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
}

void ChartView::disconnectMarkers()
{
    foreach(QLegendMarker *marker, charting->legend()->markers())
    {
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
}

void ChartView::handleMarkerClicked()
{
    QLegendMarker *marker = qobject_cast<QLegendMarker *>(sender());
    Q_ASSERT(marker);

    switch(marker->type())
    {
        case QLegendMarker::LegendMarkerTypeXY:
        {
            // Toggle visibility of series
            marker->series()->setVisible(!marker->series()->isVisible());
            marker->setVisible(true);

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

            break;
        }
        default:
        {
            qDebug() << "Unknown marker type";
            break;
        }
    }
}

void ChartView::initChart()
{
    ch[0] = new QLineSeries;
    ch[0]->setName("channal0");
    ch[1] = new QLineSeries;
    ch[1]->setName("channal2");
    ch[2] = new QLineSeries;
    ch[2]->setName("channal3");
    ch[3] = new QLineSeries;
    ch[3]->setName("channal4");

    charting = new QChart();

    charting->addSeries(ch[0]);
    charting->addSeries(ch[1]);
    charting->addSeries(ch[2]);
    charting->addSeries(ch[3]);
    charting->createDefaultAxes();

    charting->axisX()->setRange(0, 300);
    charting->axisY()->setRange(0, 1000);

    charting->setTitle("雷达实时数据");
    charting->legend()->setVisible(true);
    charting->legend()->setAlignment(Qt::AlignBottom);
    charting->setTheme(QChart::ChartThemeLight);

    setChart(charting);
    setRenderHint(QPainter::Antialiasing);
    setRubberBand(QChartView::RectangleRubberBand);
}

void ChartView::updateChart(qint8 chNum, QVector<qint32> &coor, QByteArray &data)
{
    int x_min, x_max;
    int y_min, y_max;
    int y_data;

    x_min = 0;
    x_max = coor.last() * 1.1;
    for(int i = 0; i < coor.size(); i++)
    {
        y_data = data.mid(i * 2, 2).toHex().toInt(nullptr, 16);
        ch[chNum]->append(coor[i], y_data);
        if(y_data >= y_max)
            y_max = y_data;
    }
    y_min = 0;

    y_max *= 1.1;
    charting->axisX()->setRange(x_min, x_max);
    charting->axisY()->setRange(y_min, y_max);
}
