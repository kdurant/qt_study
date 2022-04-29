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
    void parseTrackerFile(QString &path, QVector<QPointF> &track);

private:
    Ui::Navigation *ui;

    BspConfig::Gps_Info prevGpsInfo;
    double              currentSpeed{0};

    QString          mapPath;
    int              m_tile_X_offset{0};
    int              m_tile_Y_offset{0};
    QVector<QPointF> m_gps_routine;
};

#endif  // NOTEINFO_H
