#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>
#include <QtDebug>

#include "LineWithArrow.h"
#include "common.h"
#define M_PI 3.14159265358979323846

class MapView : public QGraphicsView
{
public:
    struct TileMapInfo
    {
        int min_zoom_level;
        int start_x;
        int len_x;

        int start_y;
        int len_y;

        int max_zoom_level;

        int current_zoom;
    };

public:
    MapView(QWidget *parent = 0);

    // 经纬度坐标(lng, lat)转瓦片地图坐标(tileX, tileY)
    int lng_lat2tilex(double lng, double lat, int zoom)
    {
        return ((lng + 180) / 360) * pow(2, zoom);
    }

    int lng_lat2tiley(double lng, double lat, int zoom)
    {
        return (0.5 - log(tan(lat * M_PI / 180) + 1 / cos(lat * M_PI / 180)) / (2 * M_PI)) * pow(2, zoom);
    }

    // 经纬度坐标(lng, lat)转像素坐标(pixelX, pixelY)
    int lng_lat2pixelx(double lng, double lat, int zoom)
    {
        return (int)(((lng + 180) / 360) * pow(2, zoom) * 256) % 256;
    }

    int lng_lat2pixely(double lng, double lat, int zoom)
    {
        return (int)((0.5 - log(tan(lat * M_PI / 180) + 1 / cos(lat * M_PI / 180)) / (2 * M_PI)) * pow(2, zoom) * 256) % 256;
    }

    // 瓦片地图的像素坐标(pixelX, pixelY)转经纬度坐标(lng, lat)
    double pixel2lng(int tileX, int tileY, int pixelX, int pixelY, int zoom)
    {
        return ((tileX + pixelX / 256.0) / pow(2, zoom)) * 360 - 180;
    }

    double pixel2lat(int tileX, int tileY, int pixelX, int pixelY, int zoom)
    {
        double t0 = 2 * M_PI * ((tileY + pixelY / 256.0) / (pow(2, zoom)));
        double t1 = sinh(M_PI - t0);
        return atan(t1) * 180 / M_PI;
    }

    /**
     * @brief gps_distance
     * @param lng1
     * @param lat1
     * @param lng2
     * @param lat2
     * @return distance, unit: m
     */
    double gps_distance(double lng1, double lat1, double lng2, double lat2)
    {
        double d = sqrt((lng1 - lng2) * (lng1 - lng2) + (lat1 - lat2) * (lat1 - lat2)) / 180 * M_PI * 6300000;
        return d;
    }

    /**
     * @brief 瓦片地图的GPS坐标转换为像素坐标
     * @param lng
     * @param lat
     * @return
     */
    QPoint gps2pos(double lng, double lat, int zoom_level)
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
    /**
     * @brief
     * @param path
     */

    void        setMapPath(QString &path);
    void        parseMapInfo();
    TileMapInfo getMapInfo()
    {
        return m_tileMapInfo;
    }
    void setDefaultZoom(int zoom)
    {
        m_tileMapInfo.current_zoom = zoom;
    }
    void loadMap();  // load map resource and display it

    void loadRealTimePoint(QPointF point);
    void loadTracker(QPointF start, QPointF end);
    void loadSerialNum(QPointF posi, int num);

    void deleleAllItems()
    {
        QList<QGraphicsItem *> is = scene->items();

        foreach(QGraphicsItem *item, is)
        {
            scene->removeItem(item);
        }
    }

protected:
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QGraphicsScene *scene;
    QString         m_mapPath;
    int             m_tile_X_offset{0};
    int             m_tile_Y_offset{0};
    TileMapInfo     m_tileMapInfo;
};

#endif  // MAPVIEW_H
