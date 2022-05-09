#include "MapView.h"
#include <set>

MapView::MapView(QWidget *parent)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    scene = new QGraphicsScene;
    setScene(scene);
}

void MapView::setMapPath(QString &path)
{
    m_mapPath = path;
}

void MapView::parseMapInfo()
{
    QFileInfoList list = Common::getFileList(m_mapPath);

    auto get_tile_start_x = [](QFileInfo &info) -> int
    {
        int len = info.path().split('/').length();
        return info.path().split('/')[len - 1].toInt(nullptr, 10);
    };

    auto get_tile_start_y = [](QFileInfo &info) -> int
    {
        return info.baseName().toInt(nullptr, 10);
    };

    auto get_tile_zoom = [](QFileInfo &info) -> int
    {
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
    int zoom_diff = m_tileMapInfo.current_zoom - m_tileMapInfo.min_zoom_level;
    int exp       = static_cast<int>(pow(2, zoom_diff));
    int start_x   = exp * m_tileMapInfo.start_x;
    int start_y   = exp * m_tileMapInfo.start_y;
    int len_x     = exp * m_tileMapInfo.len_x;
    int len_y     = exp * m_tileMapInfo.len_y;
    //    for(int x = 0; x < 4; ++x)
    for(int x = 0; x < len_x; ++x)
    {
        for(int y = 0; y < len_y; ++y)
        {
            QString tile = QString("%1/%2/%3/%4.png")
                               .arg(m_mapPath)
                               .arg(m_tileMapInfo.current_zoom)
                               .arg(x + start_x)
                               .arg(y + start_y);
            QGraphicsPixmapItem *map = new QGraphicsPixmapItem(QPixmap(tile));
            map->setPos(QPoint(x * 256, y * 256));
            scene->addItem(map);
        }
    }
    setScene(scene);
    update();
}

void MapView::loadRealTimePoint(QPointF point)
{
    QBrush brush;
    brush.setColor(Qt::green);
    QPen pen;
    pen.setColor(Qt::green);
    pen.setWidth(3);
    QPointF pos = gps2pos(point.x(), point.y(), m_tileMapInfo.current_zoom);
    scene->addEllipse(pos.x(), pos.y(), 3, 3, pen, brush);
}

void MapView::loadTracker(QPointF start, QPointF end)
{
    scene->addLine(QLineF(gps2pos(start.x(), start.y(), m_tileMapInfo.current_zoom), gps2pos(end.x(), end.y(), m_tileMapInfo.current_zoom)), QPen(Qt::red));
}

void MapView::loadSerialNum(QPointF posi, int num)
{
    QGraphicsTextItem *str = new QGraphicsTextItem(QString::number(num, 10));
    str->setDefaultTextColor(QColor(Qt::yellow));
    str->setPos(gps2pos(posi.x(), posi.y(), m_tileMapInfo.current_zoom) - QPoint(0, 20));
    scene->addItem(str);
    update();
}

void MapView::mousePressEvent(QMouseEvent *event)
{
    QPointF cursorPoint = event->pos();

    QPointF scenePos = mapToScene(QPoint(cursorPoint.x(), cursorPoint.y()));
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
