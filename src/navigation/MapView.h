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
#include <QMessageBox>
#include <QPolygonF>

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

    // 经纬度坐标(lng, lat)转像素坐标(pixelX, pixelY), 瓦片地图的坐标系
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
     * @brief 检查当前经维度对应的瓦片地图编号，超出已加载瓦片地图范围(正负1）的将不会显示轨迹
     *          最好调用gps2pos()前检查下
     * @param lng
     * @param lat
     * @param zoom_level
     * @return
     */
    bool checkPosValid(double lng, double lat, int zoom_level);
    /**
     * @brief 瓦片地图的GPS坐标转换为像素坐标
     * @param lng
     * @param lat
     * @return
     */
    QPoint gps2pos(double lng, double lat, int zoom_level);

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

    /**
     * @brief 将实时GPS位置显示在地图上
     *
     * @param point
     */
    void loadRealTimePoint(QPointF point);
    void loadTracker(QPointF start, QPointF end, const QPen &pen);
    void loadSerialNum(QPointF posi, int num);

    void loadPolygonF(const QPolygonF &polygon);

    /**
     * @brief 显示测区的矩形边界
     * @param rect
     */
    void loadSurveyBorder(const QRectF &rect);

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

    QGraphicsPolygonItem *m_scanPoly{nullptr};

    int         m_deviation{10};  // 实际GPS会在瓦片地图的GPS范围之外
    int         m_tile_X_offset{0};
    int         m_tile_Y_offset{0};
    TileMapInfo m_tileMapInfo{
        -1, -1, -1, -1, -1, -1, -1};
};

#endif  // MAPVIEW_H
