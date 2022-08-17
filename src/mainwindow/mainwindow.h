#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "common.h"
#include "bsp_config.h"
#include "noteinfo.h"
#include "navigation.h"

#include <QCloseEvent>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>
#include <QNetworkInterface>

#include <QSettings>
#include <QScrollBar>

#include <QDebug>
#include <QFile>
#include <QHostInfo>
#include <QMessageBox>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QStandardItemModel>

#include <QThread>

#include "ui_mainwindow.h"
#include "ui_navigation.h"
#include "radarwidget.h"

#include "AdSampleControl.h"
#include "DevInfo.h"
#include "OfflineWaveform.h"
#include "OnlineWaveform.h"
#include "ProtocolDispatch.h"
#include "UpdateBin.h"
#include "WaveExtract.h"
#include "ADControl.h"
#include "DAControl.h"

#include "laserType1.h"
#include "laserType2.h"
#include "laserType3.h"
#include "laserType4.h"
#include "laserType5.h"
#include "laserType6.h"

#include "Epos2Controller.h"
#include "PusiController.h"

#include "SaveWave.h"
#include "gpsInfo.h"
#include "AttitudeSensor.h"
#include "qcustomplot.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    struct _RadarVector_
    {
        RadarWidget                  *device;
        RadarWidget::__radar_status__ para;
    };
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void        initParameter();
    void        generateDefaultConfig();
    bool        checkConfigFile();
    QStringList read_ip_address();

    void setToolBar();

protected:
    void timerEvent(QTimerEvent *event);

private:
    Ui::MainWindow *ui;
    QSettings      *configIni;
    QStringList     localIP;
    qint32          timer1s;
    QLabel         *statusLabel;

    int     radarNumber{0};
    int     m_loop_i;  // 构造函数里声明变量会导致gdb错误
    QString item;

    QVector<_RadarVector_> radar;

    Navigation *engineerView;
    Navigation *pilotView;
    bool isLoadMap{false};

    NoteInfo   *note;

private:
    void configRadar(RadarWidget::__radar_status__ &radar)
    {
        switch(radar.radarType)
        {
            case BspConfig::RADAR_TYPE_LAND:
                radar.deviceIP   = QHostAddress("192.168.1.101");
                radar.devicePort = 5555;
                radar.name       = "陆地雷达";
                break;
            case BspConfig::RADAR_TYPE_OCEAN:
                radar.deviceIP   = QHostAddress("192.168.1.102");
                radar.devicePort = 4444;
                radar.name       = "海洋雷达";
                break;
            case BspConfig::RADAR_TYPE_DRONE:
                radar.deviceIP   = QHostAddress("192.168.1.102");
                radar.devicePort = 4444;
                radar.name       = "无人机雷达";
            case BspConfig::RADAR_TYPE_DOUBLE_WAVE:
                radar.deviceIP   = QHostAddress("192.168.1.102");
                radar.devicePort = 4444;
                radar.name       = "双波长雷达";
            case BspConfig::RADAR_TYPE_WATER_GUARD:
                radar.deviceIP   = QHostAddress("192.168.1.102");
                radar.devicePort = 4444;
                radar.name       = "水下预警雷达";
                break;
            case BspConfig::RADAR_TYPE_SECOND_INSTITUDE:
                radar.deviceIP   = QHostAddress("192.168.1.102");
                radar.devicePort = 4444;
                radar.name       = "海二所雷达";
                break;
            default:
                break;
        }
    };
};

#endif  // MAINWINDOW_H
