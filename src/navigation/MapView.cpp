#include "MapView.h"

MapView::MapView(QWidget *parent)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    scene = new QGraphicsScene;
    setScene(scene);
}

void MapView::setMapPath(QString &path, int x, int y)
{
    m_mapPath       = path;
    m_tile_X_offset = x;
    m_tile_Y_offset = y;
}

void MapView::loadMap()
{
    for(int x = 0; x < 4; ++x)
    {
        for(int y = 0; y < 4; ++y)
        {
            QString              tile = QString("%1/%2/%3.png").arg(m_mapPath).arg(x + m_tile_X_offset).arg(y + m_tile_Y_offset);
            QGraphicsPixmapItem *map  = new QGraphicsPixmapItem(QPixmap(tile));
            map->setPos(QPoint(x * 256, y * 256));
            scene->addItem(map);
        }
    }
    setScene(scene);
    update();
}

void MapView::loadTracker(QPointF start, QPointF end)
{
    scene->addLine(QLineF(gps2pos(start.x(), start.y(), 14), gps2pos(end.x(), end.y(), 14)), QPen(Qt::red));
    update();
}

void MapView::loadSerialNum(QPointF posi, int num)
{
    QGraphicsTextItem *str = new QGraphicsTextItem(QString::number(num, 10));
    str->setDefaultTextColor(QColor(Qt::yellow));
    str->setPos(gps2pos(posi.x(), posi.y(), 14) - QPoint(0, 20));
    scene->addItem(str);
    update();
}

void MapView::mousePressEvent(QMouseEvent *event)
{
    QPointF cursorPoint = event->pos();

    QPointF scenePos = mapToScene(QPoint(cursorPoint.x(), cursorPoint.y()));

    qreal viewWidth  = width();
    qreal viewHeight = height();

    qDebug() << "position of MainWindow : " << cursorPoint << "\n"
             << scenePos << "\n"
             << "ui->graphicsView->width() = " << viewWidth << "\n"
             << "ui->graphicsView->height() = " << viewHeight;
}

void MapView::wheelEvent(QWheelEvent *event)
{
    qreal scaleFactor     = this->matrix().m11();
    int   wheelDeltaValue = event->delta();
    // 向上滚动，放大;
    if(wheelDeltaValue > 0)
    {
        this->scale(1.2, 1.2);
    }
    // 向下滚动，缩小;
    else
    {
        this->scale(1.0 / 1.2, 1.0 / 1.2);
    }
    update();
}
