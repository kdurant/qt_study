#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <QWidget>
#include <QVector>
#include "bsp_config.h"
#include "common.h"
#include <QtCore>

namespace Ui
{
class Navigation;
}

class Navigation : public QWidget
{
    Q_OBJECT

public:
    struct Point
    {
        double longitude;  // 经度
        double latitude;   // 纬度
    };
    struct Line
    {
        int number;

        // Ax + By + C = 0
        double A;
        double B;
        double C;

        double          angle;
        Point           start;
        Point           end;
        QVector<double> key;    // x
        QVector<double> value;  // y
    };

    struct CurrentTracker
    {
        QVector<double> key;    // x
        QVector<double> value;  // y
    };

    explicit Navigation(QWidget *parent = 0);
    ~Navigation();

    void   initUI();
    void   initSignalSlot(void);
    void   plotSettings();
    void   addTrackerGraph(void);  // 显示实时轨迹
    void   addTargetGraph(void);   // 显示当前为止与目标的连线
    int    paserTracker(const QString &filepath);
    int    setPostion(const BspConfig::Gps_Info &gps);
    double distanceBetweenPoints(double longitude1, double latitude1, double longitude2, double latitude2);
    double distanceBetwwenPointsAndLine(double longitude, double latitude);

    double scanWidth(double height, double angle = 15);

    void showGpsInfo(const BspConfig::Gps_Info &gps);
    void showSystemInfo(double speed);
    int  getTestData(void);

private:
    Ui::Navigation *ui;

    QVector<Line>       targetGraph;  // 航线信息
    Line                historyTracker;
    CurrentTracker      realTimeTracker;
    BspConfig::Gps_Info prevGpsInfo;
    bool                hasTrackerGraph{false};
    double              currentSpeed{0};
};

#endif  // NOTEINFO_H
