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

    void showGpsInfo(const BspConfig::Gps_Info &gps);
    void showSystemInfo(double speed);
    int  getTestData(void);
    /**
    * @brief 从航迹规划文件中找到航迹的起点和终点
    *
    * @param path
    * @param track
    */
    void parseTrackerFile(QString &path, QVector<QPointF> &track);

    /**
    * @brief 将规划的轨迹分割成点的集合，计算测区覆盖率使用
    *
    * @param track
    * @param nums 每条航线将会被分割成的点数
    * @param point
    *
    * @return 
    */
    bool splitTracker(QVector<QPointF> &track, int nums, QVector<QPointF> &point);

private:
    Ui::Navigation *ui;

    BspConfig::Gps_Info prevGpsInfo;
    double              currentSpeed{0};

    QString          mapPath;
    int              m_tile_X_offset{0};
    int              m_tile_Y_offset{0};
    QVector<QPointF> m_gps_routine;
    QVector<QPointF> m_split_tracker;
};

#endif  // NOTEINFO_H
