#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("雷达控制软件"));

    dispatch    = new ProtocolDispatch();
    preview     = new PreviewProcess();
    updateFlash = new UpdateBin();

    configIni = new QSettings("../Radar/config.ini", QSettings::IniFormat);

    initParameter();
    uiConfig();

    udpBind();
    initSignalSlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initParameter()
{
    QString   localHostName = QHostInfo::localHostName();
    QHostInfo info          = QHostInfo::fromName(localHostName);
    //    if(info.addresses()[1].toString().startsWith("192.168.1"))
    //        ui->lineEdit_localIP->setText(info.addresses()[1].toString());
    //    else
    //    {
    //        ui->lineEdit_localIP->setText("请更改本机IP地址为：192.168.1.xxx");
    //        ui->lineEdit_localIP->setStyleSheet("color:red");
    //    }
    ui->lineEdit_localIP->setText(configIni->value("System/localIP").toString());
    ui->lineEdit_localPort->setText(configIni->value("System/localPort").toString());

    radarType = configIni->value("System/radarType").toString();

    configIni->setValue("Laser/freq", 1111);
    if(radarType == RADAR_TYPE_760 || radarType == RADAR_TYPE_OCEAN)
    {
        deviceIP   = QHostAddress(configIni->value("System/oceanIP").toString());
        devicePort = configIni->value("System/oceanPort").toInt();
    }
    else if(radarType == RADAR_TYPE_LAND)
    {
        deviceIP   = QHostAddress(configIni->value("System/landIP").toString());
        devicePort = configIni->value("System/landPort").toInt();
    }
    else
    {
        deviceIP   = QHostAddress(configIni->value("System/oceanIP").toString());
        devicePort = configIni->value("System/oceanPort").toInt();
    }

    //configIni->setValue("System/RadarType", "land");
    ui->lineEdit_laser_freq->setText(configIni->value("Laser/freq").toString());

    ui->lineEdit_sampleLen->setText(configIni->value("Preview/sampleLen").toString());
    ui->lineEdit_sampleRate->setText(configIni->value("Preview/sampleRate").toString());
    ui->lineEdit_firstStartPos->setText(configIni->value("Preview/firstStartPos").toString());
    ui->lineEdit_firstLen->setText(configIni->value("Preview/firstLen").toString());
    ui->lineEdit_secondStartPos->setText(configIni->value("Preview/secondStartPos").toString());
    ui->lineEdit_secondLen->setText(configIni->value("Preview/secondLen").toString());
    ui->lineEdit_sumThreshold->setText(configIni->value("Preview/sumThreshold").toString());
    ui->lineEdit_subThreshold->setText(configIni->value("Preview/subThreshold").toString());
    ui->lineEdit_compressLen->setText(configIni->value("Preview/compressLen").toString());
    ui->lineEdit_compressRatio->setText(configIni->value("Preview/compressRatio").toString());

    ui->rBtn_radarType->setChecked(true);
    ui->rBtn_radarType->setText(radarType + "雷达");
}

void MainWindow::saveParameter()
{
    //    configIni->setValue("System/RadarType", "land");
    //    configIni->setValue("Laser/freq", ui->lineEdit_laser_freq->text().toInt());
    configIni->setValue("Preview/sampleLen", ui->lineEdit_sampleLen->text().toInt());
    configIni->setValue("Preview/sampleRate", ui->lineEdit_sampleRate->text().toInt());
    configIni->setValue("Preview/firstStartPos", ui->lineEdit_firstStartPos->text().toInt());
    configIni->setValue("Preview/firstLen", ui->lineEdit_firstLen->text().toInt());
    configIni->setValue("Preview/secondStartPos", ui->lineEdit_secondStartPos->text().toInt());
    configIni->setValue("Preview/secondLen", ui->lineEdit_secondLen->text().toInt());
    configIni->setValue("Preview/compressLen", ui->lineEdit_compressLen->text().toInt());
    configIni->setValue("Preview/compressRatio", ui->lineEdit_compressRatio->text().toInt());
}

void MainWindow::uiConfig()
{
    if(radarType == RADAR_TYPE_760)
    {
        ui->label_secondStartPos->hide();
        ui->label_secondLen->hide();
        ui->label_subThreshold->hide();
        ui->label_sumThreshold->hide();
        ui->lineEdit_secondStartPos->hide();
        ui->lineEdit_secondLen->hide();
        ui->lineEdit_subThreshold->hide();
        ui->lineEdit_sumThreshold->hide();
    }
    else if(radarType == RADAR_TYPE_DOUBLE)
    {
        ui->label_subThreshold->hide();
        ui->lineEdit_subThreshold->hide();

        ui->label_sumThreshold->hide();
        ui->lineEdit_sumThreshold->hide();
        ui->tabWidget->setTabEnabled(2, false);
        ui->tabWidget->setTabEnabled(3, false);
        ui->tabWidget->setTabEnabled(4, false);
        ui->tabWidget->setTabEnabled(5, false);
    }
    ui->checkBox_autoZoom->setChecked(true);
    labelVer = new QLabel(SOFTWARE_VER);
    ui->statusBar->addPermanentWidget(labelVer);
}

void MainWindow::udpBind()
{
    udpSocket = new QUdpSocket(this);
    if(!udpSocket->bind(QHostAddress(ui->lineEdit_localIP->text()), ui->lineEdit_localPort->text().toInt()))
        QMessageBox::warning(NULL, "警告", "雷达连接失败");
    else
        ui->statusBar->showMessage(tr("连接设备成功"), 0);
    udpSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 1024 * 1024 * 1);
}

void MainWindow::processPendingDatagram()
{
    QByteArray datagram;
    int        len;
    while(udpSocket->hasPendingDatagrams())
    {
        len = udpSocket->pendingDatagramSize();
        datagram.resize(len);
        udpSocket->readDatagram(datagram.data(), datagram.size());

        dispatch->dipatchData(datagram);
    }
}

void MainWindow::processPreview()
{
    WaveData chData = preview->getSignalWave();
    for(qint8 i = 0; i < 4; i++)
    {
        QTime time;
        time.start();
        ui->graphicsView->updateChart(i, chData.ch[i].coor, chData.ch[i].data);
        qDebug() << "data num : " << chData.ch[i].coor.size() << "elapse time: " << time.elapsed() << " ms";
    }
}

void MainWindow::writeUdpatagram(qint32 command, qint32 data_len, qint32 data)
{
    QByteArray frame = dispatch->encode(command, data_len, data);
    udpSocket->writeDatagram(frame.data(), frame.size(), deviceIP, devicePort);
}

void MainWindow::writeUdpatagram(uint32_t command, uint32_t data_len, QByteArray &data)
{
    QByteArray frame = dispatch->encode(command, data_len, data);
    udpSocket->writeDatagram(frame.data(), frame.size(), deviceIP, devicePort);
}

void MainWindow::changeUIInfo(uint32_t command, QByteArray &data)
{
    switch(command)
    {
        case SlaveUp::SYS_INFO:
            ui->lineEdit_fpgaVer->setText("V" + data.mid(276, 4));
            break;
        default:
            break;
    }
}

void MainWindow::initSignalSlot()
{
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagram()));

    connect(dispatch, SIGNAL(previewDataReady(QByteArray &)), preview, SLOT(setDataFrame(QByteArray &)));
    connect(dispatch, SIGNAL(flashDataReady(QByteArray &)), updateFlash, SLOT(setDataFrame(QByteArray &)));

    connect(preview, SIGNAL(previewReadyShow()), this, SLOT(processPreview()));
    connect(preview, SIGNAL(previewParaReadySet(qint32, qint32, qint32)), this, SLOT(writeUdpatagram(qint32, qint32, qint32)));
    connect(dispatch, SIGNAL(infoDataReady(uint32_t, QByteArray &)), this, SLOT(changeUIInfo(uint32_t, QByteArray &)));

    connect(updateFlash, SIGNAL(flashCommandReadySet(uint32_t, uint32_t, QByteArray &)), SLOT(writeUdpatagram(uint32_t, uint32_t, QByteArray &)));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveParameter();
}

void MainWindow::on_actionNote_triggered()
{
    NoteInfo *note = new NoteInfo;
    note->show();
}

void MainWindow::on_pushButton_setPreviewPara_clicked()
{
    int totalSampleLen = ui->lineEdit_sampleLen->text().toInt();
    int previewRatio   = ui->lineEdit_sampleRate->text().toInt();
    int firstPos       = ui->lineEdit_firstStartPos->text().toInt();
    int firstLen       = ui->lineEdit_firstLen->text().toInt();
    int secondPos      = ui->lineEdit_secondStartPos->text().toInt();
    int secondLen      = ui->lineEdit_secondLen->text().toInt();
    int compressLen    = ui->lineEdit_compressLen->text().toInt();
    int compressRatio  = ui->lineEdit_compressRatio->text().toInt();

    if(secondPos < firstPos + firstLen)
    {
        QMessageBox::critical(NULL, "错误", "第二段起始位置需要小于第一段起始位置+第一段采样长度");
        return;
    }
    if(compressLen >= secondLen)
    {
        QMessageBox::critical(NULL, "错误", "压缩长度需要小于第二段长度");
        return;
    }
    if(secondPos + secondLen >= totalSampleLen)
    {
        QMessageBox::critical(NULL, "错误", "第二段起始位置+第二段采样长度需要小于总采样长度");
        return;
    }

    if(compressLen % compressRatio != 0)
    {
        QMessageBox::critical(NULL, "错误", "压缩长度需要是压缩比的整数倍");
        return;
    }

    preview->setTotalSampleLen(totalSampleLen);
    preview->setPreviewRatio(previewRatio);
    preview->setFirstPos(firstPos);
    preview->setFirstLen(firstLen);
    preview->setSecondPos(secondPos);
    preview->setSecondLen(secondLen);
    preview->setCompressLen(compressLen);
    preview->setCompressRatio(compressRatio);
}

void MainWindow::on_pushButton_sampleEnable_clicked()
{
    QByteArray frame;
    quint32    status;

    if(ui->pushButton_sampleEnable->text() == "开始采集")
    {
        status = 0x01010101;
        ui->pushButton_sampleEnable->setText("停止采集");
    }
    else
    {
        status = 0;
        ui->pushButton_sampleEnable->setText("开始采集");
    }
    preview->setPreviewEnable(status);
}

void MainWindow::on_pushButton_ReadInfo_clicked()
{
    QByteArray frame;

    frame = dispatch->encode(MasterSet::SYS_INFO, 4, 0x00000001);
    udpSocket->writeDatagram(frame.data(), frame.size(), deviceIP, devicePort);

    //    updateFlash->flashRead(0x00);
}

void MainWindow::on_checkBox_autoZoom_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
        ui->graphicsView->setZoomFlag(true);
    else
        ui->graphicsView->setZoomFlag(false);
}
