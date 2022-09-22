#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include <QWidget>

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
#include "Epos4Controller.h"
#include "PusiController.h"

#include "ReadHardDisk.h"
#include "gpsInfo.h"
#include "AttitudeSensor.h"
#include "qcustomplot.h"

namespace Ui
{
class RadarWidget;
}

class RadarWidget : public QWidget
{
    Q_OBJECT
public:
    struct _preview_settings__
    {
        int    laserFreq;
        double laserPower;
        int    motorSpeed;
        int    sampleLen;
        int    sampleRatio;
        int    firstPos;
        int    firstLen;
        int    secondPos;
        int    secondLen;
        int    sumThreshold;
        int    valueThreshold;

        double APDHV;
        double PMT1HV;
        double PMT2HV;
        double PMT3HV;
    };

    struct __radar_status__
    {
        BspConfig::RadarType radarType;
        QString              name;
        QString              namePrefix;
        QHostAddress         deviceIP{QHostAddress("192.168.1.102")};
        quint16              devicePort{4444};
        QString              localIP;

        _preview_settings__ previewSettings;

        int  state1{1};               // sata读写状态机
        bool ssdLinkStatus{false};    // ssd是否连接
        bool udpLinkStatus{false};    // udp是否可以正常通信
        bool adCaptureStatus{false};  // ad是否正在采集
        bool ssdStoreStatus{false};   // 是否正在存储采集数据
    } sysStatus;

public:
    RadarWidget(__radar_status__ para, QWidget *parent = nullptr);
    ~RadarWidget();

    void initParameter();
    void saveParameter();
    void uiConfig();

    void udpBind();

    void initSignalSlot();

    __radar_status__ &getRadarStatus(void)
    {
        return sysStatus;
    }

    void plotLineSettings(void);
    void plotColormapSettings(void);
    void updateColormap(int chart, int angle, const QVector<double> &key, const QVector<double> &data);

    void initFileListUi(void);
    void motorResponse(MotorController::MOTOR_STATUS status);

    void displayColorMap(const QVector<WaveExtract::WaveformInfo> &allCh);

protected:
    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);

signals:
    void sampleDataReady(BspConfig::RadarType type, const QVector<quint8> &sampleData);
    void startPaserSampleNumber();
    void sendGpsInfo(BspConfig::Gps_Info &data);

private slots:

    void processPendingDatagram();

    void on_bt_showWave_clicked();

    void getSysInfo();
    void showLaserInfo(LaserType4::LaserInfo &info);

    void showSampleData(const QVector<WaveExtract::WaveformInfo> &allCh, int status);

private:
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

    Ui::RadarWidget *ui;
    QSettings       *configUser;

    QUdpSocket   *udpSocket;
    QThread      *thread;
    qint32        timer1s;
    QElapsedTimer elapsedTimer;
    qint32        timerRefreshUI;
    bool          refreshUIFlag{false};
    bool          refreshRadarFlag{false};

    // QString     localIP;
    quint16 localPort{6666};

    int8_t     fpgaRadarType{-1};  // fpga 内部固化的雷达类型
    QByteArray fpgaVersion{5, char(0)};

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

    MotorController *motorController;
    EPOS2           *epos2Driver;

    DevInfo                   *devInfo;
    QVector<DevInfo::ParaInfo> sysParaInfo;

    QAction *m_pActionCopy;

    ReadHardDisk   *ssd;
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

    qint64           testCnt{0};
    QVector<QString> testString;
};

#endif  // MAIN_WIDGET_H
