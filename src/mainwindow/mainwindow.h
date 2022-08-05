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
    void updateColormap(int chart, int angle, const QVector<double> &key, const QVector<double> &data);

    void initFileListUi(void);
    void motorResponse(MotorController::MOTOR_STATUS status);

protected:
    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);

signals:
    void sampleDataReady(BspConfig::RadarType type, const QVector<quint8> &sampleData);
    void startPaserSampleNumber();

private slots:

    void processPendingDatagram();

    void on_actionNote_triggered();

    void on_bt_showWave_clicked();

    void getSysInfo();
    void showLaserInfo(LaserType4::LaserInfo &info);

    QString read_ip_address(void);
    void    showSampleData(const QVector<WaveExtract::WaveformInfo> &allCh, int status);

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

    struct _preview_settings__
    {
        int laserFreq;
        int sampleLen;
        int sampleRatio;
        int firstPos;
        int firstLen;
        int secondPos;
        int secondLen;
        int sumThreshold;
        int valueThreshold;
    } previewSettings{-1, -1, -1, -1, -1, -1, -1, -1, -1};

    struct DoubleWaveConfig
    {
        double prev_angle;
        double step_ratio;
        double min_view_angle;
        double max_view_angle;

        int  sampleCnt;
        bool rescale[4];

        QList<QVector<WaveExtract::WaveformInfo>> data;
    } doubleWaveConfig;

    Ui::MainWindow *ui;
    QUdpSocket     *udpSocket;
    QSettings      *configIni;
    QThread        *thread;
    qint32          timer1s;
    QElapsedTimer   elapsedTimer;
    qint32          timerRefreshUI;
    bool            refreshUIFlag{false};
    bool            refreshRadarFlag{false};

    BspConfig::RadarType radarType;

    QString localIP;
    quint16 localPort{6666};

    QHostAddress deviceIP;
    quint16      devicePort;
    int8_t       fpgaRadarType{-1};  // fpga 内部固化的雷达类型
    QByteArray   fpgaVersion{5, char(0)};

    QFile binFile;

    ProtocolDispatch *dispatch;

    AdSampleControll *preview;
    bool              autoZoomPlot{true};
    OnlineWaveform   *onlineWaveForm;
    OfflineWaveform  *offlineWaveForm;
    bool              running;
    WaveExtract      *waveExtract;
    QVector<quint8>   sampleData;

    DAControl *daDriver;
    ADControl *adDriver;

    UpdateBin *updateFlash;

    LaserController *laserDriver{nullptr};
    LaserType1      *laser1Driver;
    LaserType2      *laser2Driver;
    LaserType6      *laser6Driver;

    MotorController *motorController;
    EPOS2           *epos2Driver;

    DevInfo                   *devInfo;
    QVector<DevInfo::ParaInfo> sysParaInfo;

    SaveWave       *ssd;
    GpsInfo        *gps;
    AttitudeSensor *attitude;

    QList<QCustomPlot *> widget2CustomPlotList;
    QList<QCPColorMap *> widget2QCPColorMapList;

    // RADAR_TYPE_WATER_GUARD 用的变量
    struct WaterGuard
    {
        bool                                        startSaveBase;
        bool                                        isSavedBase;  // 以及保存了用于比较的数据
        int                                         videoMemoryDepth;
        WaveExtract::MOTOR_CNT_STATE                state;
        QVector<QVector<WaveExtract::WaveformInfo>> base;
        QVector<WaveExtract::WaveformInfo>          diff;
    };

    int colorMap_X_max{512};

    WaterGuard waterGuard;
    QMutex     mutex;

    NoteInfo   *note;
    Navigation *engineerView;
    Navigation *pilotView;

    qint64           testCnt{0};
    QVector<QString> testString;

#ifdef TEST_NAV
    QQueue<QPointF> gps_test_pos;
#endif
};

#endif  // MAINWINDOW_H
