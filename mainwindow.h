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

#include <QMessageBox>
#include <QHostInfo>
#include <QDebug>

#include <QThread>

#include "./src/Protocol/RadarProtocolBasic.h"
#include "./src/Protocol/DoubleWaveProtocol.h"

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

protected:
    void closeEvent(QCloseEvent *event);

private slots:

    void processPendingDatagram();

    void on_actionNote_triggered();

    void on_pushButton_setPreviewPara_clicked();

    void on_pushButton_sampleEnable_clicked();

    void on_pushButton_laserInfo_clicked();

    void on_pushButton_ReadInfo_clicked();

    void on_checkBox_autoZoom_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QUdpSocket *    udpSocket;
    QSettings *     configIni;

    QString      radarType;
    QHostAddress deviceIP;
    quint16      devicePort;

    QLabel *labelVer;

    DoubleWaveProtocol *protocol;
    ProtocolResult      para;
    //    DoubleWaveProtocol  rotocol;
};

#endif  // MAINWINDOW_H
