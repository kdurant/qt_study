#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "bsp_config.h"
#include "noteinfo.h"
#include "radarchart.h"
#include "protocol.h"

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

extern QQueue<QByteArray>   adOrigData;    // 原始数据流
extern QQueue<QVector<int>> adSingleData;  // 处理好的单次可显示数据

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

private:
    Ui::MainWindow *ui;
    QUdpSocket *    udpSocket;

    Protocol    p;
    RadarChart *chart;
};

#endif  // MAINWINDOW_H
