#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <QWidget>
#include <QVector>
#include "bsp_config.h"
#include "common.h"
#include <QtCore>
#include <QFileDialog>
#include <QMessageBox>
#include "MapView.h"
#include "LineWithArrow.h"
#include "AirArea.h"

namespace Ui
{
class Navigation;
}

class Navigation : public QWidget
{
    Q_OBJECT
public:
    explicit Navigation(QWidget *parent = 0);
    ~Navigation();

    void initUI();
    void initSignalSlot(void);

    void updateGpsInfo(BspConfig::Gps_Info &data);
    void showGpsInfo(const BspConfig::Gps_Info &gps);
    void showSystemInfo(void);
    void setScanAngle(double angle);
    bool getLoadMapInfo(void) const 
{ 
            return isLoadedMap && isLoadedTracker;
        }

protected:
    void timerEvent(QTimerEvent *event);

private:
    Ui::Navigation *ui;
    qint32          timer1s;

    QString mapPath;
    int     m_tile_X_offset{0};
    int     m_tile_Y_offset{0};

    QVector<QPointF> m_realtime_path;  // 保存GPS实时轨迹数据，地图缩放时，需要重新绘制已有轨迹
    AirArea          m_designedAirArea;

    int m_gpsInterval{0};

    bool isLoadedMap{false};
    bool isLoadedTracker{false};
};

#endif  // NOTEINFO_H
