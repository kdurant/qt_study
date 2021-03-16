#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

#include "Epos2Controller.h"

#include "SaveWave.h"

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

    void plotSettings(void);

protected:
    void closeEvent(QCloseEvent *event);

private slots:

    void processPendingDatagram();

    void on_actionNote_triggered();

    void on_btnNorFlashRead_clicked();

    void on_btnNorFlashErase_clicked();

    void on_btnNorFlasshReadFile_clicked();

    void on_bt_showWave_clicked();

    void getSysInfo();

    QString read_ip_address(void);

private:
    struct __sys_status__
    {
        bool ssdLinkStatus;
        bool udpLinkStatus;
        bool adCaptureStatus;

        QLabel *label_udpLinkStatus;
        QLabel *label_ssdLinkStatus;
        QLabel *label_adCaptureStatus;
    } sysStatus;

    Ui::MainWindow *ui;
    QUdpSocket *    udpSocket;
    QSettings *     configIni;
    QThread *       thread;

    BspConfig::RadarType radarType;

    QString localIP;
    quint16 localPort{6666};

    QHostAddress deviceIP;
    quint16      devicePort;

    ProtocolDispatch *dispatch;

    AdSampleControll *preview;
    bool              autoZoomPlot{true};
    OnlineWaveform *  onlineWaveForm;

    DAControl *daDriver;
    ADControl *adDriver;

    UpdateBin *updateFlash;

    quint32 sampleFrameNumber;

    OfflineWaveform *offlineWaveForm;
    bool             running;

    LaserController *laserDriver;
    LaserType1 *     laser1Driver;
    LaserType2 *     laser2Driver;
    LaserType3 *     laser3Driver;

    EPOS2 *epos2Driver;

    DevInfo *                  devInfo;
    QVector<DevInfo::ParaInfo> sysParaInfo;
    QTimer *                   autoReadInfoTimer;

    SaveWave *ssd;
};

#endif  // MAINWINDOW_H
