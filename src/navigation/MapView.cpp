#include "MapView.h"
#include <set>

MapView::MapView(QWidget *parent)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    scene = new QGraphicsScene;
    setScene(scene);
}

bool MapView::checkPosValid(double lng, double lat, int zoom_level)
{
    int zoom_diff = m_tileMapInfo.current_zoom - m_tileMapInfo.min_zoom_level;
    int exp       = static_cast<int>(pow(2, zoom_diff));
    int start_x   = exp * m_tileMapInfo.start_x;
    int start_y   = exp * m_tileMapInfo.start_y;

    int tileX = lng_lat2tilex(lng, lat, zoom_level);
    int tileY = lng_lat2tiley(lng, lat, zoom_level);

    if(tileX < start_x - m_deviation || tileX > start_x + m_tileMapInfo.len_x * exp + m_deviation)
        return false;

    if(tileY < start_y - m_deviation || tileY > start_y + m_tileMapInfo.len_y * exp + m_deviation)
        return false;

    return true;
}

QPoint MapView::gps2pos(double lng, double lat, int zoom_level)
{
    int zoom_diff = m_tileMapInfo.current_zoom - m_tileMapInfo.min_zoom_level;
    int exp       = static_cast<int>(pow(2, zoom_diff));
    int start_x   = exp * m_tileMapInfo.start_x;
    int start_y   = exp * m_tileMapInfo.start_y;

    int tileX  = lng_lat2tilex(lng, lat, zoom_level);
    int tileY  = lng_lat2tiley(lng, lat, zoom_level);
    int pixelX = lng_lat2pixelx(lng, lat, zoom_level);
    int pixelY = lng_lat2pixely(lng, lat, zoom_level);
    int pos_x  = (tileX - start_x) * 256 + pixelX;
    int pos_y  = (tileY - start_y) * 256 + pixelY;

    return QPoint(pos_x, pos_y);
}

void MapView::setMapPath(QString &path)
{
    m_mapPath = path;
}

void MapView::parseMapInfo()
{
    QFileInfoList list = Common::getFileList(m_mapPath);
    if(list.length() == 0)
        QMessageBox::critical(this, "error", "地图路径错误");

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
    brush.setColor(QColor(0, 255, 0, 127));
    QPen pen;
    pen.setColor(QColor(0, 255, 0, 127));
    pen.setWidth(3);

    if(checkPosValid(point.x(), point.y(), m_tileMapInfo.current_zoom) == false)
        return;

    QPointF pos = gps2pos(point.x(), point.y(), m_tileMapInfo.current_zoom);
    centerOn(pos);
    scene->addEllipse(pos.x(), pos.y(), 3, 3, pen, brush);
}

void MapView::loadTracker(QPointF start, QPointF end, const QPen &pen)
{
    scene->addLine(QLineF(gps2pos(start.x(), start.y(), m_tileMapInfo.current_zoom), gps2pos(end.x(), end.y(), m_tileMapInfo.current_zoom)), pen);
}

void MapView::loadSerialNum(QPointF posi, int num)
{
    QGraphicsTextItem *str = new QGraphicsTextItem(QString::number(num, 10));
    str->setDefaultTextColor(QColor(Qt::magenta));
    str->setPos(gps2pos(posi.x(), posi.y(), m_tileMapInfo.current_zoom) - QPoint(0, 20));
    scene->addItem(str);
    update();
}

// 需要将经维度转换为屏幕坐标
void MapView::loadPolygonF(const QPolygonF &polygon)
{
    //    if(m_scanPoly != nullptr)
    //        scene->removeItem(m_scanPoly);
    QPolygonF trans;
    for(auto &p : polygon)
    {
        trans.append(gps2pos(p.x(), p.y(), m_tileMapInfo.current_zoom));
    }
    m_scanPoly = scene->addPolygon(trans, QPen(QColor(0, 150, 0, 0)), QBrush(QColor(0, 150, 0, 100)));
}

void MapView::loadRect(const QRectF &rect)
{
    QRectF r;
    r.setTopLeft(gps2pos(rect.topLeft().x(), rect.topLeft().y(), m_tileMapInfo.current_zoom));
    r.setBottomRight(gps2pos(rect.bottomRight().x(), rect.bottomRight().y(), m_tileMapInfo.current_zoom));
    scene->addRect(r);
}

void MapView::mousePressEvent(QMouseEvent *event)
{
    QPointF cursorPoint = event->pos();

    QPointF scenePos = mapToScene(QPoint(cursorPoint.x(), cursorPoint.y()));
}

void MapView::wheelEvent(QWheelEvent *event)
{
    double scaleFactor = pow(2., event->angleDelta().y() / 240.0);

    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if(factor < 0.07 || factor > 100)
        return;
    scale(scaleFactor, scaleFactor);
}
