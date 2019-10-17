#include "./inc/chartview.h"
#include <QChartView>

QT_CHARTS_USE_NAMESPACE

// void ChartView::mousePressEvent(QMouseEvent *event)
// {  //鼠标左键按下，记录beginPoint
//     if(event->button() == Qt::LeftButton)
//         beginPoint = event->pos();
//     QChartView::mousePressEvent(event);
// }
//
// void ChartView::mouseMoveEvent(QMouseEvent *event)
// {  //鼠标移动事件
//     QPoint point;
//     point = event->pos();
//
//     emit mouseMovePoint(point);
//     QChartView::mouseMoveEvent(event);
// }
//
// void ChartView::mouseReleaseEvent(QMouseEvent *event)
// {
//     if(event->button() == Qt::LeftButton)
//     {  //鼠标左键释放，获取矩形框的endPoint,进行缩放
//         endPoint = event->pos();
//         QRectF rectF;
//         rectF.setTopLeft(this->beginPoint);
//         rectF.setBottomRight(this->endPoint);
//         this->chart()->zoomIn(rectF);
//     }
//     else if(event->button() == Qt::RightButton)
//         this->chart()->zoomReset();  //鼠标右键释放，resetZoom
//     QChartView::mouseReleaseEvent(event);
// }

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

ChartView::ChartView(QWidget *parent)
    : QChartView(parent)
{
    this->setDragMode(QGraphicsView::RubberBandDrag);
    //    this->setRubberBand(QChartView::RectangleRubberBand);//设置为矩形选择方式
    //    this->setRubberBand(QChartView::VerticalRubberBand);
    //    this->setRubberBand(QChartView::HorizontalRubberBand);

    this->setMouseTracking(true);  //必须开启此功能

    initChart();
    connectMarkers();
}

ChartView::~ChartView()
{
    // 必须保留此函数
}

void ChartView::initChart()
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

    charting = new QChart();

    charting->addSeries(channal1);
    charting->addSeries(channal2);
    charting->addSeries(channal3);
    charting->addSeries(channal4);
    charting->createDefaultAxes();

    charting->axisX()->setRange(0, 300);
    charting->axisY()->setRange(0, 1000);

    charting->setTitle("雷达实时数据");
    charting->legend()->setVisible(true);
    charting->legend()->setAlignment(Qt::AlignBottom);
    charting->setTheme(QChart::ChartThemeBlueCerulean);

    setChart(charting);
    setRenderHint(QPainter::Antialiasing);
    setRubberBand(QChartView::RectangleRubberBand);
}

void ChartView::updateChart(AD_Data &data)
{
    channal1->clear();
    qint32 x = 0, y = 0;
    x = data.first_start_pos;
    for(int i = 0; i < data.first_data.size(); i += 4)
    {
        y = data.first_data.mid(i, 4).toInt(nullptr, 16);
        channal1->append(x, y);
        x++;
    }
}

void ChartView::connectMarkers()
{
    //![1]
    // Connect all markers to handler
    foreach(QLegendMarker *marker, charting->legend()->markers())
    {
        // Disconnect possible existing connection to avoid multiple connections
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
        QObject::connect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
    //![1]
}

void ChartView::disconnectMarkers()
{
    //![2]
    foreach(QLegendMarker *marker, charting->legend()->markers())
    {
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
    //![2]
}

void ChartView::handleMarkerClicked()
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
