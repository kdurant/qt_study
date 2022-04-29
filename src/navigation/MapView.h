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
#define M_PI 3.14159265358979323846

class MapView : public QGraphicsView
{
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
     * @brief 瓦片地图的GPS坐标转换为像素坐标
     * @param lng
     * @param lat
     * @return
     */
    QPoint gps2pos(double lng, double lat)
    {
        int tileX  = lng_lat2tilex(lng, lat, 14);
        int tileY  = lng_lat2tiley(lng, lat, 14);
        int pixelX = lng_lat2pixelx(lng, lat, 14);
        int pixelY = lng_lat2pixely(lng, lat, 14);
        int pos_x  = (tileX - m_tile_X_offset) * 256 + pixelX;
        int pos_y  = (tileY - m_tile_Y_offset) * 256 + pixelY;

        return QPoint(pos_x, pos_y);
    };

    void setMapPath(QString &path, int x, int y);
    void loadMap();
    void loadTracker(QPointF start, QPointF end);
    void loadSerialNum(QPointF posi, int num);

protected:
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QGraphicsScene *scene;
    QString         m_mapPath;
    int             m_tile_X_offset{0};
    int             m_tile_Y_offset{0};
};

#endif  // MAPVIEW_H
