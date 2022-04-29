#include "MapView.h"

MapView::MapView(QWidget *parent)
{
    //    resize(1024, 1024);
    //    scale(0.5, 0.5);

    scene = new QGraphicsScene;

    //    QString base_path = "D:/map1/14";
    //    int     base_x    = 13186;  //3292;
    //    int     base_y    = 7342;   //1836;
    //    for(int x = 0; x < 4; ++x)
    //    {
    //        for(int y = 0; y < 4; ++y)
    //        {
    //            QString              tile = QString("%1/%2/%3.png").arg(base_path).arg(x + base_x).arg(y + base_y);
    //            QGraphicsPixmapItem *map  = new QGraphicsPixmapItem(QPixmap(tile));
    //            map->setPos(QPoint(x * 256, y * 256));
    //            scene->addItem(map);

    //            //            QGraphicsTextItem *text = new QGraphicsTextItem(QString("%1_%2").arg(base_x + x).arg(base_y + y));
    //            //            text->setDefaultTextColor(QColor(Qt::red));
    //            //            text->setPos(QPoint(x * 256 + 128, y * 256 + 128));
    //            //            scene->addItem(text);
    //        }
    //    }
    //    //    scene->addL
    //    //    scene->addPath()

    //    double lng{109.36858809};
    //    double lat{18.24587404};

    //    auto draw_point = [&](double lng, double lat) -> QPoint {
    //        int tileX  = lng_lat2tilex(lng, lat, 12);
    //        int tileY  = lng_lat2tiley(lng, lat, 12);
    //        int pixelX = lng_lat2pixelx(lng, lat, 12);
    //        int pixelY = lng_lat2pixely(lng, lat, 12);
    //        int pos_x  = (tileX - base_x) * 256 + pixelX;
    //        int pos_y  = (tileY - base_y) * 256 + pixelY;

    //        return QPoint(pos_x, pos_y);
    //        //        scene->addEllipse(QRect(QPoint(pos_x, pos_y), QSize(5, 5)), QPen(Qt::red));
    //    };

    //    LineWithArrow *line = new LineWithArrow(draw_point(109.36189716, 18.22506045), draw_point(109.57428520, 18.15901267));
    //    scene->addItem(line);
    //    scene->addLine(QLineF(draw_point(109.36189716, 18.22506045), draw_point(109.57428520, 18.15901267)));

    //    scene->addEllipse(QRect(draw_point(lng, lat), QSize(5, 5)), QPen(Qt::red));
    //    scene->addEllipse(QRect(draw_point(109.37851763, 18.23153185), QSize(5, 5)), QPen(Qt::red));

    setScene(scene);
    //    update();
    //    ui->graphicsView->setSceneRect(0, 0, 100, 100);
    //    show();
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

            //            QGraphicsTextItem *text = new QGraphicsTextItem(QString("%1_%2").arg(base_x + x).arg(base_y + y));
            //            text->setDefaultTextColor(QColor(Qt::red));
            //            text->setPos(QPoint(x * 256 + 128, y * 256 + 128));
            //            scene->addItem(text);
        }
    }
    setScene(scene);
    update();
}

void MapView::loadTracker(QPointF start, QPointF end)
{
    scene->addLine(QLineF(gps2pos(start.x(), start.y()), gps2pos(end.x(), end.y())), QPen(Qt::red));
    update();
}

void MapView::loadSerialNum(QPointF posi, int num)
{
    QGraphicsTextItem *str = new QGraphicsTextItem(QString::number(num, 10));
    str->setDefaultTextColor(QColor(Qt::yellow));
    //    str->setTextWidth(5);
    str->setPos(gps2pos(posi.x(), posi.y()) - QPoint(0, 20));
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
