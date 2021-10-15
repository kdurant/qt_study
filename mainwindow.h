#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "common.h"
#include "bsp_config.h"
#include "noteinfo.h"

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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initParameter();
    void saveParameter();
    void uiConfig();

    void udpBind();

    void initSignalSlot();
    void setToolBar(void);

    void plotLineSettings(void);
    void plotColormapSettings(void);
    void updateColormap(QVector<WaveExtract::WaveformInfo> &allCh);

    void initSysInfoUi(void);
    void initFileListUi(void);

    void showSampleData(QVector<quint8> &sampleData);

protected:
    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);

private slots:

    void processPendingDatagram();

    void on_actionNote_triggered();

    void on_bt_showWave_clicked();

    void getSysInfo();
    void showLaserInfo(LaserType4::LaserInfo &info);

    QString read_ip_address(void);

private:
    struct __sys_status__
    {
        bool ssdLinkStatus;    // ssd是否连接
        bool udpLinkStatus;    // udp是否可以正常通信
        bool adCaptureStatus;  // ad是否正在采集
        bool ssdStoreStatus;   // 是否正在存储采集数据

        QLabel *label_udpLinkStatus;
        QLabel *label_ssdStoreStatus;
        QLabel *label_adCaptureStatus;
    } sysStatus;

    Ui::MainWindow *ui;
    QUdpSocket *    udpSocket;
    QSettings *     configIni;
    QThread *       thread;
    qint32          timer1s;
    qint32          timerRefreshUI;
    bool            refreshUIFlag{false};

    BspConfig::RadarType radarType;

    QString localIP;
    quint16 localPort{6666};

    QHostAddress deviceIP;
    quint16      devicePort;

    ProtocolDispatch *dispatch;

    AdSampleControll *preview;
    bool              autoZoomPlot{true};
    OnlineWaveform *  onlineWaveForm;
    OfflineWaveform * offlineWaveForm;
    bool              running;

    DAControl *daDriver;
    ADControl *adDriver;

    UpdateBin *updateFlash;

    LaserController *laserDriver;
    LaserType1 *     laser1Driver;
    LaserType2 *     laser2Driver;
    LaserType3 *     laser3Driver;
    LaserType4 *     laser4Driver;
    LaserType5 *     laser5Driver;
    LaserType6 *     laser6Driver;

    EPOS2 *epos2Driver;

    DevInfo *                  devInfo;
    QVector<DevInfo::ParaInfo> sysParaInfo;

    SaveWave *      ssd;
    GpsInfo *       gps;
    AttitudeSensor *attitude;

    QList<QCustomPlot *> widget2CustomPlotList;
    QList<QCPColorMap *> widget2QCPColorMapList;

    // RADAR_TYPE_WATER_GUARD 用的变量
    struct WaterGuard
    {
        bool                                        startSaveBase;
        bool                                        isSavedBase;
        bool                                        isValidRange;  // 保证从0°开始保存数据
        int                                         videoMemoryDepth;
        WaveExtract::MOTOR_CNT_STATE                state;
        QVector<QVector<WaveExtract::WaveformInfo>> base;
        QVector<WaveExtract::WaveformInfo>          diff;
    };

    WaterGuard waterGuard;

    qint64 testCnt{0};
};

#endif  // MAINWINDOW_H
