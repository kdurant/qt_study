#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <QWidget>
#include <QVector>
#include "bsp_config.h"
#include "common.h"
#include <QtCore>
#include <QFileDialog>
#include <QMessageBox>

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

private:
    Ui::Navigation *ui;

    BspConfig::Gps_Info prevGpsInfo;
    double              currentSpeed{0};

    QString mapPath;
};

#endif  // NOTEINFO_H
