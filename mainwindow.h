#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "bsp_config.h"
#include "noteinfo.h"

#include <QCloseEvent>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>

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

#include "ProtocolDispatch.h"
#include "AdSampleControl.h"
#include "UpdateBin.h"
#include "OfflineWaveform.h"

#include "laserType1.h"
#include "laserType2.h"

#include "Epos2Controller.h"

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

    void getDeviceVersion(QString &version);

    void adadaf();

protected:
    void closeEvent(QCloseEvent *event);

private slots:

    void processPendingDatagram();

    void on_actionNote_triggered();

    void on_pushButton_setPreviewPara_clicked();

    void on_pushButton_sampleEnable_clicked();

    void on_pushButton_ReadInfo_clicked();

    void on_checkBox_autoZoom_stateChanged(int arg1);

    void on_btnNorFlashRead_clicked();

    void on_btnNorFlashErase_clicked();

    void on_btnNorFlasshReadFile_clicked();

    void on_bt_showWave_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket *    udpSocket;
    QSettings *     configIni;

    BspConfig::RadarType radarType;
    QHostAddress         deviceIP;
    quint16              devicePort;

    QLabel *labelVer;

    ProtocolDispatch *dispatch;
    AdSampleControll *preview;
    UpdateBin *       updateFlash;
    QThread *         thread;

    quint32 sampleFrameNumber;

    OfflineWaveform *offlineWaveForm;
    bool             running;

    LaserController *laserDriver;
    LaserType1 *     laser1Driver;
    LaserType2 *     laser2Driver;

    EPOS2 *epos2Driver;
};

#endif  // MAINWINDOW_H
