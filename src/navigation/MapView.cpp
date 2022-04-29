#include "MapView.h"
#include <set>

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

void MapView::parseMap()
{
    QFileInfoList list = Common::getFileList(m_mapPath);

    auto get_tile_start_x = [](QFileInfo &info) -> int {
        int len = info.path().split('/').length();
        return info.path().split('/')[len - 1].toInt(nullptr, 10);
    };

    auto get_tile_start_y = [](QFileInfo &info) -> int {
        return info.baseName().toInt(nullptr, 10);
    };

    auto get_tile_zoom = [](QFileInfo &info) -> int {
        int len = info.path().split('/').length();
        return info.path().split('/')[len - 2].toInt(nullptr, 10);
    };

    m_tileMapInfo.min_zoom_level = get_tile_zoom(list[0]);
    m_tileMapInfo.start_x        = get_tile_start_x(list[0]);
    m_tileMapInfo.start_y        = get_tile_start_y(list[0]);

    std::set<int> set_x, set_y, zoom;
    foreach(QFileInfo info, list)
    {
        int z = get_tile_zoom(info);
        zoom.insert(z);
        if(z == m_tileMapInfo.min_zoom_level)
        {
            set_x.insert(get_tile_start_x(info));
            set_y.insert(get_tile_start_y(info));
        }
    }
    m_tileMapInfo.len_x          = *set_x.rbegin() - *set_x.begin() + 1;
    m_tileMapInfo.len_y          = *set_y.rbegin() - *set_y.begin() + 1;
    m_tileMapInfo.max_zoom_level = *zoom.rbegin();
    return;
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
