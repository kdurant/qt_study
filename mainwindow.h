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

#include <QThread>

#include "./src/Protocol/ProtocolDispatch.h"
#include "./src/Protocol/PreviewProcess.h"
#include "./src/UpdateBin/UpdateBin.h"

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
    /**
     * @brief processPreview
     * 已经收到了一个完整的预览数据包，需要显示了
     */
    void processPreview();

    void writeUdpatagram(qint32 command, qint32 data_len, qint32 data);
    void writeUdpatagram(uint32_t command, uint32_t data_len, QByteArray &data);

    void changeUIInfo(uint32_t command, QByteArray &data);

    void on_actionNote_triggered();

    void on_pushButton_setPreviewPara_clicked();

    void on_pushButton_sampleEnable_clicked();

    void on_pushButton_ReadInfo_clicked();

    void on_checkBox_autoZoom_stateChanged(int arg1);

    void on_btnNorFlashRead_clicked();

    void on_btnNorFlashErase_clicked();

    void on_btnNorFlasshReadFile_clicked();

    void on_bt_selectShowFile_clicked();
    void getFrameNumber();

private:
    Ui::MainWindow *ui;
    QUdpSocket *    udpSocket;
    QSettings *     configIni;

    QString      radarType;
    QHostAddress deviceIP;
    quint16      devicePort;

    QLabel *labelVer;

    ProtocolDispatch *dispatch;
    PreviewProcess *  preview;
    UpdateBin *       updateFlash;
    QThread *thread;

    quint32 sampleFrameNumber;
    QString showFileName;
};

#endif  // MAINWINDOW_H
