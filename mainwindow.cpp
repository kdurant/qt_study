#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), configIni(new QSettings("./config.ini", QSettings::IniFormat)), thread(new QThread())
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);

    dispatch        = new ProtocolDispatch();
    preview         = new AdSampleControll();
    updateFlash     = new UpdateBin();
    offlineWaveForm = new OfflineWaveform();
    onlineWaveForm  = new OnlineWaveform();

    daDriver = new DAControl();
    adDriver = new ADControl();

    laserDriver  = new LaserController();
    laser1Driver = new LaserType1();
    laser2Driver = new LaserType2();
    laser3Driver = new LaserType3();
    laser4Driver = new LaserType4();

    epos2Driver = new EPOS2();

    devInfo = new DevInfo();

    timer1s = startTimer(1000);

    ssd = new SaveWave();

    gps      = new GpsInfo();
    attitude = new AttitudeSensor;

    sysStatus.ssdLinkStatus   = false;
    sysStatus.udpLinkStatus   = false;
    sysStatus.adCaptureStatus = false;
    sysStatus.ssdStoreStatus  = false;

    offlineWaveForm->moveToThread(thread);
    connect(thread, SIGNAL(started()), offlineWaveForm, SLOT(getADsampleNumber()));
    connect(offlineWaveForm, SIGNAL(finishSampleFrameNumber()), thread, SLOT(quit()));

    //    connect(waveShow, SIGNAL(finishSampleFrameNumber()), waveShow, SLOT(deleteLater()));
    //    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    initParameter();
    uiConfig();

    udpBind();
    initSignalSlot();
    setToolBar();

    plotLineSettings();
    plotColormapSettings();
    devInfo->getSysPara(sysParaInfo);
    initSysInfoUi();
    initFileListUi();
    getSysInfo();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initParameter()
{
    if(!configIni->contains("System/radarType"))
    {
        QMessageBox::warning(this, "warning", "缺少配置文件");
    }

    //    radarType = configIni->value("System/radarType").toInt();
    switch(configIni->value("System/radarType").toInt())
    {
        case 0:
            radarType = BspConfig::RADAR_TPYE_OCEAN;
            break;
        case 1:
            radarType = BspConfig::RADAR_TPYE_LAND;
            break;
        case 2:
            radarType = BspConfig::RADAR_TPYE_760;
            break;
        case 3:
            radarType = BspConfig::RADAR_TPYE_DOUBLE_WAVE;
            break;
        case 4:
            radarType = BspConfig::RADAR_TPYE_DRONE;
            break;
        case 5:
            radarType = BspConfig::RADAR_TPYE_UNDER_WATER;
            break;
        default:
            radarType = BspConfig::RADAR_TPYE_OCEAN;
            QMessageBox::critical(this, "error", "请设置正确的雷达类型");
            break;
    }

    QString   localHostName = QHostInfo::localHostName();
    QHostInfo info          = QHostInfo::fromName(localHostName);

    if(configIni->value("System/mode").toString() == "debug")
        localIP = "127.0.0.1";
    else
    {
        localIP = read_ip_address();
        if(!localIP.contains("192.168.1"))
        {
            QMessageBox::warning(this, "警告", "请修改主机IP地址(192.168.1.xxx)");
            ui->statusBar->showMessage(tr("请修改主机IP地址(192.168.1.xxx"), 3);
        }
    }

    localPort = configIni->value("System/localPort").toUInt();

    if(configIni->value("System/mode").toString() == "debug")
        deviceIP = QHostAddress("127.0.0.1");
    else
        deviceIP = QHostAddress(configIni->value("System/radarIP").toString());
    devicePort = configIni->value("System/radarPort").toInt();

    if(devicePort == 0)
    {
        QMessageBox::critical(this, "error", "请在配置文件中指定设备IP地址和端口号");
        return;
    }

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
}

void MainWindow::saveParameter()
{
    return;
}

void MainWindow::uiConfig()
{
    ui->treeWidget_attitude->expandAll();
    ui->treeWidget_attitude->resizeColumnToContents(0);
    QList<QTreeWidgetItem *> itemList;
    itemList = ui->treeWidget_attitude->findItems("姿态传感器", Qt::MatchExactly);
    itemList.first()->setHidden(true);

    QRegExp           decReg("[0-9]+$");
    QRegExpValidator *decValidator = new QRegExpValidator(decReg, this);
    QRegExp           floatReg("[0-9\.]+$");
    QRegExpValidator *floatValidator = new QRegExpValidator(floatReg, this);
    QRegExp           hexReg("[0-9,a-f,A-F]+$");
    QRegExpValidator *hexValidator = new QRegExpValidator(hexReg, this);

    ui->lineEdit_motorTargetSpeed->setValidator(decValidator);
    ui->lineEdit_DAValue->setValidator(floatValidator);
    ui->lineEdit_sampleLen->setValidator(decValidator);
    ui->lineEdit_sampleRate->setValidator(decValidator);
    ui->lineEdit_firstStartPos->setValidator(decValidator);
    ui->lineEdit_firstLen->setValidator(decValidator);
    ui->lineEdit_secondStartPos->setValidator(decValidator);
    ui->lineEdit_secondLen->setValidator(decValidator);
    ui->lineEdit_sumThreshold->setValidator(decValidator);
    ui->lineEdit_subThreshold->setValidator(decValidator);
    ui->lineEdit_compressLen->setValidator(decValidator);
    ui->lineEdit_compressRatio->setValidator(decValidator);
    ui->lineEdit_ssdSearchStartUnit->setValidator(hexValidator);
    ui->lineEdit_cameraFreq->setValidator(decValidator);
    ui->lineEdit_pmtDelayTime->setValidator(decValidator);
    ui->lineEdit_pmtGateTime->setValidator(decValidator);

    ui->label_compressLen->hide();
    ui->lineEdit_compressLen->hide();
    ui->label_compressRatio->hide();
    ui->lineEdit_compressRatio->hide();
    ui->lineEdit_pmtDelayTime->hide();
    ui->lineEdit_pmtGateTime->hide();
    ui->label_laserPower->hide();
    ui->comboBox_laserPower->hide();

    ui->tabWidget->setTabEnabled(4, false);
    ui->tabWidget->setTabEnabled(5, false);

    if(radarType == BspConfig::RADAR_TPYE_760)
    {
        setWindowTitle(tr("760雷达控制软件"));
        ui->lineEdit_radarType->setText("760雷达");
        ui->label_secondStartPos->hide();
        ui->label_secondLen->hide();
        ui->label_subThreshold->hide();
        ui->label_sumThreshold->hide();
        ui->lineEdit_secondStartPos->hide();
        ui->lineEdit_secondLen->hide();
        ui->lineEdit_subThreshold->hide();
        ui->lineEdit_sumThreshold->hide();
    }
    else if(radarType == BspConfig::RADAR_TPYE_DOUBLE_WAVE)
    {
        setWindowTitle(tr("双波长雷达控制软件"));
        ui->lineEdit_radarType->setText("双波长雷达");
        ui->label_subThreshold->hide();
        ui->lineEdit_subThreshold->hide();

        ui->label_sumThreshold->hide();
        ui->lineEdit_sumThreshold->hide();
    }
    else if(radarType == BspConfig::RADAR_TPYE_OCEAN)
    {
        setWindowTitle(tr("海洋雷达控制软件"));
        ui->lineEdit_radarType->setText("海洋雷达");
        ui->label_laserCurrent->hide();
        ui->lineEdit_laserCurrent->hide();
        ui->comboBox_laserFreq->addItem("5000");
        //        ui->label
    }
    else if(radarType == BspConfig::RADAR_TPYE_LAND)
    {
        setWindowTitle(tr("陆地雷达控制软件"));
        ui->lineEdit_radarType->setText("陆地雷达");
        ui->label_triggerMode->hide();
        ui->rbtn_triggerInside->hide();
        ui->rbtn_triggerOutside->hide();
        ui->comboBox_laserFreq->addItem("100000");
        ui->comboBox_laserFreq->addItem("200000");
        ui->comboBox_laserFreq->addItem("400000");
    }
    else if(radarType == BspConfig::RADAR_TPYE_DRONE)
    {
        setWindowTitle(tr("无人机雷达控制软件"));
        ui->lineEdit_radarType->setText("无人机雷达");
        ui->label_laserPower->hide();
        ui->comboBox_laserPower->hide();
        ui->comboBox_laserFreq->addItem("4000");

        ui->tabWidget->setTabEnabled(4, true);

        ui->lineEdit_laserCurrent->setToolTip("3500 <= current <=4500");
        ui->lineEdit_laserCurrent->setValidator(new QIntValidator(0, 1000, this));

        QStringList DA1List{"APDHV", "PMT1HV", "PMT2HV", "PMT3HV"};
        QStringList AD1List{"APD TEMP", "APDHV FB", "PMT1HV FB", "PMT2HV FB", "PMT3HV FB"};
        ui->comboBox_DAChSelect->addItems(DA1List);
        ui->comboBox_ADChSelect->addItems(AD1List);

        ui->label_laserCurrent->hide();
        ui->lineEdit_laserCurrent->hide();
        ui->btn_laserSetCurrent->hide();
    }
    else if(radarType == BspConfig::RADAR_TPYE_UNDER_WATER)
    {
        setWindowTitle(tr("水下预警雷达控制软件"));
        ui->lineEdit_radarType->setText("水下预警雷达");

        QStringList DA1List{"APDHV", "PMT1HV", "PMT2HV", "PMT3HV"};
        QStringList AD1List{"APD TEMP", "APDHV FB", "PMT1HV FB", "PMT2HV FB", "PMT3HV FB"};
        ui->comboBox_DAChSelect->addItems(DA1List);
        ui->comboBox_ADChSelect->addItems(AD1List);

        QList<QTreeWidgetItem *> itemList;
        itemList = ui->treeWidget_attitude->findItems("姿态传感器", Qt::MatchExactly);
        itemList.first()->setHidden(false);

        ui->lineEdit_pmtDelayTime->show();
        ui->lineEdit_pmtGateTime->show();
    }
    else
    {
        setWindowTitle(tr("[xx]雷达控制软件"));
    }
    ui->checkBox_autoZoom->setChecked(true);

    //    sysStatus.label_udpLinkStatus = new QLabel("通信连接状态: 未连接");
    //    ui->statusBar->addPermanentWidget(sysStatus.label_udpLinkStatus);

    sysStatus.label_ssdStoreStatus = new QLabel("SSD存储状态: 停止存储");
    ui->statusBar->addPermanentWidget(sysStatus.label_ssdStoreStatus);

    sysStatus.label_adCaptureStatus = new QLabel("采集状态：停止采集");
    sysStatus.label_adCaptureStatus->setObjectName("captureStatus");
    sysStatus.label_adCaptureStatus->setStyleSheet("color:red");
    ui->statusBar->addPermanentWidget(sysStatus.label_adCaptureStatus);

    QLabel *labelVer = new QLabel();
    labelVer->setText("软件版本：v" + QString(SOFT_VERSION) + "_" + GIT_DATE + "_" + GIT_HASH);
    ui->statusBar->addPermanentWidget(labelVer);

    QList<QTreeWidgetItem *> topItems;
    QList<QTreeWidgetItem *> subItems;
    switch(radarType)
    {
        case BspConfig::RADAR_TPYE_LAND:
            break;
        case BspConfig::RADAR_TPYE_DRONE:
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "外触发频率(Hz)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "内触发频率(Hz)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "工作时间(s)"));
            ui->treeWidget_laser->addTopLevelItems(topItems);

            ui->treeWidget_laser->resizeColumnToContents(0);

            break;
        case BspConfig::RADAR_TPYE_UNDER_WATER:
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "电流(mA)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "激光头温度(°)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "LD温度(°)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "激光晶体温度(°)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "倍频晶体温度(°)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "状态位"));
            topItems.last()->addChild(new QTreeWidgetItem(topItems.last(), QStringList() << "是否打开"));
            topItems.last()->addChild(new QTreeWidgetItem(topItems.last(), QStringList() << "触发方式"));
            topItems.last()->addChild(new QTreeWidgetItem(topItems.last(), QStringList() << "自检状态"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "错误位"));
            ui->treeWidget_laser->addTopLevelItems(topItems);

            ui->treeWidget_laser->resizeColumnToContents(0);
            break;
        default:
            break;
    }
}

void MainWindow::udpBind()
{
    udpSocket = new QUdpSocket(this);
    if(!udpSocket->bind(QHostAddress(localIP), localPort))
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

        dispatch->parserFrame(datagram);
    }
}

void MainWindow::initSignalSlot()
{
    // 处理udp接收到的数据
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagram()));

    // 发送已经打包好的数据
    connect(dispatch, &ProtocolDispatch::frameDataReady, this, [this](QByteArray frame) {
        udpSocket->writeDatagram(frame.data(), frame.size(), deviceIP, devicePort);
    });

    connect(dispatch, &ProtocolDispatch::errorDataReady, this, [this](QString &error) {
        ui->statusBar->showMessage(error, 3);
    });

    /*
     * 读取系统参数信息相关逻辑 
     */
    connect(devInfo, &DevInfo::sendDataReady, dispatch, &ProtocolDispatch::encode);
    connect(dispatch, &ProtocolDispatch::infoDataReady, devInfo, &DevInfo::setNewData);
    connect(ui->btn_ReadSysInfo, &QPushButton::pressed, this, [this]() {
        getSysInfo();
    });

    /*
     * 波形预览相关逻辑
     */
    connect(preview, &AdSampleControll::sendDataReady, dispatch, &ProtocolDispatch::encode);

    connect(ui->btn_setPreviewPara, &QPushButton::pressed, this, [this]() {
        int     totalSampleLen = ui->lineEdit_sampleLen->text().toInt();
        int     previewRatio   = ui->lineEdit_sampleRate->text().toInt();
        int     firstPos       = ui->lineEdit_firstStartPos->text().toInt();
        int     firstLen       = ui->lineEdit_firstLen->text().toInt();
        int     secondPos      = ui->lineEdit_secondStartPos->text().toInt();
        int     secondLen      = ui->lineEdit_secondLen->text().toInt();
        int     sumThreshold   = ui->lineEdit_sumThreshold->text().toInt();
        int     valueThreshold = ui->lineEdit_subThreshold->text().toInt();
        quint16 pmtDelayTime   = ui->lineEdit_pmtDelayTime->text().toUInt();
        quint16 pmtGateTime    = ui->lineEdit_pmtGateTime->text().toUInt();

        if(secondPos < firstPos + firstLen)
        {
            QMessageBox::critical(NULL, "错误", "第二段起始位置需要小于第一段起始位置+第一段采样长度");
            return;
        }
        if(secondPos + secondLen >= totalSampleLen)
        {
            QMessageBox::critical(NULL, "错误", "第二段起始位置+第二段采样长度需要小于总采样长度");
            return;
        }

        preview->setTotalSampleLen(totalSampleLen);
        preview->setPreviewRatio(previewRatio);
        preview->setFirstPos(firstPos);
        preview->setFirstLen(firstLen);
        preview->setSecondPos(secondPos);
        preview->setSecondLen(secondLen);
        preview->setSumThreshold(sumThreshold);

        if(radarType == BspConfig::RADAR_TPYE_UNDER_WATER)
        {
            preview->setValueThreshold(valueThreshold);
            preview->setPmtDelayAndGateTime(pmtDelayTime, pmtGateTime);
        }
    });

    connect(ui->btn_sampleEnable, &QPushButton::pressed, this, [this]() {
        QByteArray frame;
        quint32    status;

        if(ui->btn_sampleEnable->text() == "开始采集")
        {
            status = 0x01010101;
            ui->btn_sampleEnable->setText("停止采集");
        }
        else
        {
            if(sysStatus.ssdStoreStatus)
            {
                QMessageBox::warning(NULL, "警告", "请先停止存储数据");
                return;
            }
            status = 0;
            ui->btn_sampleEnable->setText("开始采集");
        }
        preview->setPreviewEnable(status);
    });

    connect(dispatch,
            &ProtocolDispatch::onlineDataReady,
            onlineWaveForm,
            &OnlineWaveform::setNewData);
    connect(onlineWaveForm, &OnlineWaveform::fullSampleDataReady, this, [this](QByteArray &data) {
        QVector<quint8> sampleData;
        for(auto &i : data)  // 数据格式转换
            sampleData.append(i);
        QVector<WaveExtract::WaveformInfo> allCh;

        WaveExtract::getWaveform(radarType, sampleData, allCh);
        for(int n = 0; n < allCh.size(); n++)
        {
            if(allCh.size() != 8)  // 只有第一段波形
            {
                ui->sampleDataPlot->graph(n * 2)->setData(allCh[n].pos, allCh[n].value);
                ui->sampleDataPlot->graph(n * 2 + 1)->data().data()->clear();
            }
            else
                ui->sampleDataPlot->graph(n)->setData(allCh[n].pos, allCh[n].value);
        }
        if(autoZoomPlot)
            ui->sampleDataPlot->rescaleAxes();
        ui->sampleDataPlot->replot();

        updateColormap(allCh);
    });

    /*
     * 图表控制相关逻辑
     */
    connect(ui->checkBox_autoZoom, &QCheckBox::stateChanged, this, [this](int state) {
        if(state == Qt::Checked)
            autoZoomPlot = true;
        else
            autoZoomPlot = false;
    });
    connect(ui->checkBox_ch0Enable, &QCheckBox::stateChanged, this, [this](int state) {
        bool status = state == Qt::Unchecked ? false : true;
        ui->sampleDataPlot->graph(0)->setVisible(status);
        ui->sampleDataPlot->graph(1)->setVisible(status);
        ui->sampleDataPlot->replot();
    });
    connect(ui->checkBox_ch1Enable, &QCheckBox::stateChanged, this, [this](int state) {
        bool status = state == Qt::Unchecked ? false : true;
        ui->sampleDataPlot->graph(2)->setVisible(status);
        ui->sampleDataPlot->graph(3)->setVisible(status);
        ui->sampleDataPlot->replot();
    });
    connect(ui->checkBox_ch2Enable, &QCheckBox::stateChanged, this, [this](int state) {
        bool status = state == Qt::Unchecked ? false : true;
        ui->sampleDataPlot->graph(4)->setVisible(status);
        ui->sampleDataPlot->graph(5)->setVisible(status);
        ui->sampleDataPlot->replot();
    });
    connect(ui->checkBox_ch3Enable, &QCheckBox::stateChanged, this, [this](int state) {
        bool status = state == Qt::Unchecked ? false : true;
        ui->sampleDataPlot->graph(6)->setVisible(status);
        ui->sampleDataPlot->graph(7)->setVisible(status);
        ui->sampleDataPlot->replot();
    });

    connect(ui->btn_axisRangeSet, &QPushButton::pressed, this, [this]() {
        quint32 xMin = ui->lineEdit_axisXmin->text().toUInt(nullptr, 10);
        quint32 xMax = ui->lineEdit_axisXmax->text().toUInt(nullptr, 10);
        ui->sampleDataPlot->xAxis->setRange(xMin, xMax);

        quint32 yMin = ui->lineEdit_axisYmin->text().toUInt(nullptr, 10);
        quint32 yMax = ui->lineEdit_axisYmax->text().toUInt(nullptr, 10);
        ui->sampleDataPlot->yAxis->setRange(yMin, yMax);

        ui->sampleDataPlot->replot();
    });
    /*
     * 离线显示数据波形相关逻辑
     */
    connect(ui->btn_selectOfflineFile, &QPushButton::pressed, this, [this]() {
        QString showFileName = QFileDialog::getOpenFileName(this, tr(""), "", tr("*.bin"));  //选择路径
        if(showFileName.size() == 0)
            return;
        ui->lineEdit_selectShowFile->setText(showFileName);
        offlineWaveForm->setWaveFile(showFileName);
        thread->start();
    });

    connect(offlineWaveForm, &OfflineWaveform::sendSampleFrameNumber, this, [this](qint32 number) {
        ui->lineEdit_validFrameNum->setText(QString::number(number));
    });

    connect(offlineWaveForm, &OfflineWaveform::sendSampleFrameNumber, this, [this](qint32 number) {
        ui->slider_framePos->setMaximum(number);
        ui->spin_framePos->setMaximum(number);
    });
    connect(ui->spin_framePos,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            ui->slider_framePos,
            &QSlider::setValue);

    connect(ui->slider_framePos,
            static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged),
            ui->spin_framePos,
            &QSpinBox::setValue);

    connect(ui->bt_showWave, SIGNAL(pressed()), this, SLOT(on_bt_showWave_clicked()));
    connect(ui->btn_stopShowWave, SIGNAL(pressed()), this, SLOT(on_bt_showWave_clicked()));
    connect(ui->btn_showMotorCnt, &QPushButton::pressed, this, [this]() {
        QVector<double> motorCnt = offlineWaveForm->getMotorCnt();
        QVector<double> x;
        for(int i = 0; i < motorCnt.size(); i++)
            x.append(i);

        ui->sampleDataPlot->graph(0)->setData(x, motorCnt);
        ui->sampleDataPlot->rescaleAxes();
        ui->sampleDataPlot->replot();
    });

    connect(ui->btn_showGpsSubTime, &QPushButton::pressed, this, [this]() {
        QVector<double> y = offlineWaveForm->getGpsSubTime();
        QVector<double> x;
        for(int i = 0; i < y.size(); i++)
            x.append(i);

        ui->sampleDataPlot->graph(2)->setData(x, y);
        ui->sampleDataPlot->rescaleAxes();
        ui->sampleDataPlot->replot();
    });
    /*
     * Nor Flash操作，远程更新相关逻辑
     */

    connect(dispatch, &ProtocolDispatch::flashDataReady, updateFlash, &UpdateBin::setDataFrame);
    connect(ui->btnNorFlashWrite, &QPushButton::pressed, this, [this]() {
        uint32_t addr;
        if(ui->rBtnDecAddr->isChecked())
        {
            addr = ui->lineEdit_NorFlashStartAddr->text().toInt(nullptr, 10);
        }
        else
        {
            addr = ui->lineEdit_NorFlashStartAddr->text().toInt(nullptr, 16);
        }
        QByteArray data;
        for(int i = 0; i < 256; i++)
        {
            data.append(i);
        }
        ui->btnNorFlashWrite->setEnabled(false);
        updateFlash->flashErase(addr);

        updateFlash->flashWrite(addr, data);
        ui->btnNorFlashWrite->setEnabled(true);
    });

    connect(ui->btn_selectUpdateFile, &QPushButton::pressed, this, [this]() {
        QString updateFilePath = QFileDialog::getOpenFileName(this, tr(""), "", tr("*.bin"));  //选择路径
        if(updateFilePath.size() == 0)
            return;
        ui->lineEdit_updateFilePath->setText(updateFilePath);
    });

    connect(ui->btn_startUpdate, &QPushButton::pressed, this, [this]() {
        QString updateFilePath = ui->lineEdit_updateFilePath->text();
        if(updateFilePath.isEmpty())
        {
            QMessageBox::warning(this, "warning", "请先选择文件");
            return;
        }
        ui->pBar_updateBin->setMaximum(QFile(updateFilePath).size() - 1);
        updateFlash->flashUpdate(updateFilePath);
    });

    connect(updateFlash, &UpdateBin::updatedBytes, this, [this](qint32 bytes) {
        qDebug() << "has write " << bytes;
        ui->pBar_updateBin->setValue(bytes);
    });

    /*
     * 激光器相关处理
     */

    if(radarType == BspConfig::RADAR_TPYE_OCEAN)
    {
        connect(laser2Driver, &LaserController::sendDataReady, dispatch, &ProtocolDispatch::encode);
        connect(dispatch, &ProtocolDispatch::laserDataReady, laser2Driver, &LaserType2::setNewData);
    }
    else if(radarType == BspConfig::RADAR_TPYE_DRONE)
    {
        connect(laser3Driver, &LaserController::sendDataReady, dispatch, &ProtocolDispatch::encode);
        connect(dispatch, &ProtocolDispatch::laserDataReady, laser3Driver, &LaserType3::setNewData);
        connect(laser3Driver, &LaserType3::laserInfoReady, this, [this](LaserType3::LaserInfo &info) {
            QList<QTreeWidgetItem *> itemList;

            itemList = ui->treeWidget_laser->findItems("外触发频率(Hz)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.freq_outside));

            itemList = ui->treeWidget_laser->findItems("内触发频率(Hz)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.freq_inside));

            itemList = ui->treeWidget_laser->findItems("工作时间(s)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.work_time));
        });
    }
    else if(radarType == BspConfig::RADAR_TPYE_UNDER_WATER)
    {
        connect(laser4Driver, &LaserController::sendDataReady, dispatch, &ProtocolDispatch::encode);
        connect(dispatch, &ProtocolDispatch::laserDataReady, laser4Driver, &LaserType4::setNewData);
        connect(laser4Driver, &LaserType4::laserInfoReady, this, &MainWindow::showLaserInfo);
    }

    connect(ui->btn_laserOpen, &QPushButton::pressed, this, [this]() {
        bool status = false;
        switch(radarType)
        {
            case BspConfig::RADAR_TPYE_LAND:
                laser2Driver->setFreq(100000);
                status = laser2Driver->open();
                break;
            case BspConfig::RADAR_TPYE_DRONE:
                laser3Driver->setFreq(4000);
                status = laser3Driver->open();
                break;
            case BspConfig::RADAR_TPYE_UNDER_WATER:
                laser4Driver->setFreq(5000);
                status = laser4Driver->open();
                break;
            default:
                break;
        }
        if(!status)
            QMessageBox::warning(this, "警告", "指令流程异常，请尝试重新发送");
    });

    connect(ui->btn_laserClose, &QPushButton::pressed, this, [this]() {
        bool status = false;
        switch(radarType)
        {
            case BspConfig::RADAR_TPYE_LAND:
                status = laser2Driver->close();
                break;
            case BspConfig::RADAR_TPYE_DRONE:
                status = laser3Driver->close();
                break;
            case BspConfig::RADAR_TPYE_UNDER_WATER:
                status = laser4Driver->close();
                break;
            default:
                break;
        }
        if(!status)
            QMessageBox::warning(this, "警告", "指令流程异常，请尝试重新发送");
    });

    connect(ui->btn_laserSetCurrent, &QPushButton::pressed, this, [this]() {
        bool status = false;
        switch(radarType)
        {
            case BspConfig::RADAR_TPYE_LAND:
                status = laser2Driver->setCurrent(ui->lineEdit_laserCurrent->text().toInt());
                break;
            case BspConfig::RADAR_TPYE_DRONE:
                status = laser3Driver->setCurrent(ui->lineEdit_laserCurrent->text().toInt() / 10);
                break;
            case BspConfig::RADAR_TPYE_UNDER_WATER:
                status = laser4Driver->setPower(ui->lineEdit_laserCurrent->text().toInt() / 10);
                break;
            default:
                break;
        }
        if(status == false)
            QMessageBox::warning(this, "警告", "指令流程异常，请尝试重新发送");
    });

    connect(ui->rbtn_triggerInside, &QRadioButton::clicked, this, [this]() {
        bool status = false;
        switch(radarType)
        {
            case BspConfig::RADAR_TPYE_LAND:
                status = laser2Driver->setMode(LaserController::IN_SIDE);
                break;
            case BspConfig::RADAR_TPYE_DRONE:
                status = laser3Driver->setMode(LaserController::IN_SIDE);
                break;
            case BspConfig::RADAR_TPYE_UNDER_WATER:
                status = laser4Driver->setMode(LaserController::IN_SIDE);
                break;
            default:
                break;
        }
        if(status == false)
            QMessageBox::warning(this, "警告", "指令流程异常，请尝试重新发送");
    });

    connect(ui->rbtn_triggerOutside, &QRadioButton::clicked, this, [this]() {
        bool status = false;
        switch(radarType)
        {
            case BspConfig::RADAR_TPYE_LAND:
                status = laser2Driver->setMode(LaserController::OUT_SIDE);
                break;
            case BspConfig::RADAR_TPYE_DRONE:
                status = laser3Driver->setMode(LaserController::OUT_SIDE);
                break;
            case BspConfig::RADAR_TPYE_UNDER_WATER:
                status = laser4Driver->setMode(LaserController::OUT_SIDE);
                break;
            default:
                break;
        }
        if(status == false)
            QMessageBox::warning(this, "警告", "指令流程异常，请尝试重新发送");
    });

    connect(ui->btn_laserReadInfo, &QPushButton::pressed, this, [this]() {
        laser3Driver->getStatus();
    });

    /*
     * 电机相关逻辑
     */
    connect(epos2Driver, SIGNAL(sendDataReady(qint32, qint32, QByteArray &)), dispatch, SLOT(encode(qint32, qint32, QByteArray &)));

    connect(dispatch, &ProtocolDispatch::motorDataReady, epos2Driver, &EPOS2::setNewData);
    connect(ui->btn_motorReadSpeed, &QPushButton::pressed, this, [this]() {
        qint32 speed = 0;
        speed        = epos2Driver->getActualVelocity();
        ui->lineEdit_motorShowSpeed->setText(QString::number(speed, 10));
    });

    connect(ui->btn_motorReadPosition, &QPushButton::pressed, this, [this]() {
        quint32 postion = 0;
        postion         = epos2Driver->getActualPosition();
        ui->lineEdit_motorShowPosition->setText(QString::number(postion, 10));
    });

    connect(ui->btn_motorStart, &QPushButton::pressed, this, [this]() {
        quint16 speed = ui->lineEdit_motorTargetSpeed->text().toInt(nullptr, 10);

        epos2Driver->run(speed);
    });

    connect(ui->btn_motorInit, &QPushButton::pressed, this, [this]() {
        ui->btn_motorInit->setEnabled(false);
        epos2Driver->init();
        ui->label_motorInfo->setText("电机初始化已完成");
        ui->btn_motorInit->setEnabled(true);
    });

    connect(ui->btn_motorMoveHome, &QPushButton::pressed, this, [this]() {
        ui->btn_motorMoveHome->setEnabled(false);
        epos2Driver->moveToHome();
        ui->label_motorInfo->setText("Home模式后需要重新初始化，电机才能运动");
        ui->btn_motorMoveHome->setEnabled(true);
    });

    connect(ui->btn_motorMovePostion, &QPushButton::pressed, this, [this]() {
        quint32 position = ui->lineEdit_motorTargetPosition->text().toUInt();
        epos2Driver->moveToPosition(position);
    });

    /*
     * 采集数据保存相关逻辑
     */
    connect(ssd, SIGNAL(sendDataReady(qint32, qint32, QByteArray &)), dispatch, SLOT(encode(qint32, qint32, QByteArray &)));
    connect(dispatch, &ProtocolDispatch::ssdDataReady, ssd, &SaveWave::setNewData);
    connect(ui->btn_ssdSearchSpace, &QPushButton::pressed, this, [this]() {
        if(sysStatus.ssdStoreStatus)
        {
            QMessageBox::warning(this, "warning", "不能在写文件时检索数据");
            return;
        }
        ui->btn_ssdSearchSpace->setEnabled(false);

        ui->tableWidget_fileList->clearContents();
        ui->tableWidget_fileList->setRowCount(1);

        SaveWave::ValidFileInfo fileInfo;
        quint32                 startUnit = ui->lineEdit_ssdSearchStartUnit->text().toUInt();

        ssd->inquireSpace(startUnit, fileInfo);

        switch(fileInfo.status)
        {
            case SaveWave::FileStatus::FILE_INFO_FULL:
                ui->lineEdit_ssdAvailFileUnit->setText(QString::number(fileInfo.fileUnit + 2, 16));
                ui->lineEdit_ssdAvailDataUnit->setText(QString::number(fileInfo.endUnit + 1, 16));
                break;
            case SaveWave::FileStatus::FILE_POSITION_ERROR:
                QMessageBox::warning(this, "warning", "上次文件信息写入出错, 请断电更换硬盘！");
                break;
            case SaveWave::FileStatus::FILE_INFO_NONE:
                QMessageBox::warning(this, "warning", "没有读取到硬盘数据，确认网络连接正常");
                break;
        }

        ui->btn_ssdSearchSpace->setEnabled(true);

        // 显示已经查询到的文件信息
    });
    connect(ssd, &SaveWave::fileDataReady, this, [this](SaveWave::ValidFileInfo &fileInfo) {
        int row = ui->tableWidget_fileList->rowCount();
        ui->tableWidget_fileList->setCellWidget(row - 1, 0, new QLabel(fileInfo.name));

        ui->tableWidget_fileList->setCellWidget(row - 1, 1, new QLabel(QString::number(fileInfo.startUnit)));
        ui->tableWidget_fileList->setCellWidget(row - 1, 2, new QLabel(QString::number(fileInfo.endUnit)));
        ui->tableWidget_fileList->setCellWidget(row - 1, 3, new QLabel(QString::number(fileInfo.startUnit, 16)));
        ui->tableWidget_fileList->setCellWidget(row - 1, 4, new QLabel(QString::number(fileInfo.endUnit, 16)));

        quint32 fileSize = (fileInfo.endUnit - fileInfo.startUnit) * 16;
        QString size     = QString("%1GB / %2MB").arg(fileSize / 1024.0 / 1024).arg(fileSize / 1024.0);
        ui->tableWidget_fileList->setCellWidget(row - 1, 5, new QLabel(size));
        ui->tableWidget_fileList->setRowCount(row + 1);
    });

    connect(ui->btn_ssdEnableStore, &QPushButton::pressed, this, [this]() {
        if(!sysStatus.adCaptureStatus)
        {
            QMessageBox::warning(this, "warning", "请先开始采集");
            return;
        }

        ui->btn_ssdEnableStore->setEnabled(false);

        quint32 fileUnit = ui->lineEdit_ssdAvailFileUnit->text().toUInt(nullptr, 16);
        QString fileName = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
        if(ui->lineEdit_ssdStoreFileName->text().length() != 0)
            fileName.append(ui->lineEdit_ssdStoreFileName->text().length());
        ssd->setSaveFileName(fileUnit, fileName);

        quint32 dataUnit = ui->lineEdit_ssdAvailDataUnit->text().toUInt(nullptr, 16);
        ssd->setSaveFileAddr(dataUnit);
        ssd->enableStoreFile(0x01);
    });

    connect(ui->btn_ssdDisableStore, &QPushButton::pressed, this, [this]() {
        QMessageBox message(QMessageBox::NoIcon, "停止存储", "真的要停止存储吗", QMessageBox::Yes | QMessageBox::No, NULL);
        if(message.exec() == QMessageBox::No)
            return;
        ui->btn_ssdEnableStore->setEnabled(true);
        ssd->enableStoreFile(0x00);
    });

    /*
     * DA设置相关逻辑
     */
    connect(daDriver, SIGNAL(sendDataReady(qint32, qint32, QByteArray &)), dispatch, SLOT(encode(qint32, qint32, QByteArray &)));
    connect(ui->btn_DASetValue, &QPushButton::pressed, this, [this]() {
        if(ui->lineEdit_DAValue->text().isEmpty())
        {
            QMessageBox::warning(this, "warning", "请输入有效数据");
            return;
        }
        quint32 chNum       = ui->comboBox_DAChSelect->currentIndex();
        double  analogValue = ui->lineEdit_DAValue->text().toDouble(nullptr);
        qint32  digitValue  = 0;
        switch(radarType)
        {
            case BspConfig::RADAR_TPYE_LAND:
                digitValue = static_cast<quint32>((analogValue - 3.434) / 0.017);
                break;
            case BspConfig::RADAR_TPYE_OCEAN:
            case BspConfig::RADAR_TPYE_DRONE:
            case BspConfig::RADAR_TPYE_UNDER_WATER:
                if(chNum == 0)
                    digitValue = static_cast<qint32>((analogValue - 8.208) / 0.113);
                else if(chNum == 1)
                    digitValue = static_cast<qint32>((analogValue + 0.007) / 0.001);
                else if(chNum == 2)
                    digitValue = static_cast<qint32>((analogValue - 0.003) / 0.001);
                else if(chNum == 3)
                    digitValue = static_cast<qint32>((analogValue + 0.002) / 0.001);
                break;
            default:
                break;
        }
        if(digitValue < 0)
            digitValue = 0;
        daDriver->setChannalValue(chNum, digitValue);
        ui->plainTextEdit_DASetLog->appendPlainText(ui->comboBox_DAChSelect->currentText() + ": " + ui->lineEdit_DAValue->text() + "V");
    });

    connect(ui->btn_DAClearAll, &QPushButton::pressed, this, [this]() {
        for(int i = 0; i < 4; i++)
        {
            daDriver->setChannalValue(i, 0);
            ui->plainTextEdit_DASetLog->appendPlainText(ui->comboBox_DAChSelect->itemText(i) + ": 0V");
        }
    });

    /*
     * AD设置相关逻辑
     */
    connect(dispatch, &ProtocolDispatch::ADDataReady, adDriver, &ADControl::setNewData);
    connect(adDriver, &ADControl::sendDataReady, dispatch, &ProtocolDispatch::encode);
    connect(ui->btn_ADReadValue, &QPushButton::pressed, this, [this]() {
        quint32 chNum       = ui->comboBox_ADChSelect->currentIndex();
        qint32  digitValue  = adDriver->getChannalValue(chNum);
        double  analogValue = 0;
        if(digitValue < 0)
            ui->lineEdit_ADValue->setText(QString::number(digitValue, 10));
        else
        {
            switch(chNum)
            {
                case 0:
                    analogValue = (digitValue / 4095.00) * 5;
                    break;
                case 1:
                    analogValue = (digitValue - 12.95) / 817.8;
                    break;
                case 2:
                    analogValue = (digitValue - 1.712) / 817.4;
                    break;
                case 3:
                    analogValue = (digitValue - 0.634) / 818.4;
                    break;
                case 4:
                    analogValue = (digitValue + 1.392) / 822.7;
                    break;
            }
            ui->lineEdit_ADValue->setText(QString::number(analogValue, 'g', 2));
        }
        ui->plainTextEdit_ADSetLog->appendPlainText(ui->comboBox_ADChSelect->currentText() + ": " + ui->lineEdit_ADValue->text() + "V");
    });

    connect(ui->btn_ADReadAll, &QPushButton::pressed, this, [this]() {
        QMessageBox::warning(this, "warning", "还未实现此功能");
    });

    /*
     * gps信息处理
     */
    connect(dispatch, &ProtocolDispatch::gpsDataReady, gps, &GpsInfo::parserGpsData);
    connect(gps, &GpsInfo::gpsDataReady, this, [this](BspConfig::Gps_Info &data) {
        QList<QTreeWidgetItem *> itemList;

        itemList = ui->treeWidget_attitude->findItems("GPS信息", Qt::MatchExactly);
        itemList.first()->child(0)->setText(1, QString::number(data.week));
        itemList.first()->child(1)->setText(1, QString::number(data.current_week_ms));
        itemList.first()->child(2)->setText(1, QString::number(data.latitude, 'g', 6));
        itemList.first()->child(3)->setText(1, QString::number(data.longitude, 'g', 6));
        itemList.first()->child(4)->setText(1, QString::number(data.altitude, 'g', 6));
        itemList.first()->child(5)->setText(1, QString::number(data.roll, 'g', 6));
        itemList.first()->child(6)->setText(1, QString::number(data.pitch, 'g', 6));
        itemList.first()->child(7)->setText(1, QString::number(data.heading, 'g', 6));
    });

    connect(ui->btn_cameraFreq, &QPushButton::pressed, this, [this]() {
        uint32_t second = ui->lineEdit_cameraFreq->text().toUInt(nullptr, 10);
        if(second > 4)
        {
            QMessageBox::warning(this, "warning", "最大拍照间隔4s，请重新设置");
        }
        uint32_t   value = second * 1000000000 / 8;
        QByteArray frame = BspConfig::int2ba(value);
        dispatch->encode(MasterSet::CAMERA_FREQ_SET, 4, frame);
    });

    /*
     * 姿态传感器信息处理
     */
    connect(dispatch, &ProtocolDispatch::attitudeDataReady, attitude, &AttitudeSensor::parserFrame);
    connect(attitude, &AttitudeSensor::sendAttitudeResult, this, [this](AttitudeSensor::AttitudeInfo accelerate, AttitudeSensor::AttitudeInfo angularVelocity, AttitudeSensor::AttitudeInfo angular, AttitudeSensor::AttitudeInfo magneticField) {
        QList<QTreeWidgetItem *> itemList;

        itemList = ui->treeWidget_attitude->findItems("姿态传感器", Qt::MatchExactly);
        itemList.first()->child(0)->child(0)->setText(1, QString::number(accelerate.x, 'g', 6));
        itemList.first()->child(0)->child(1)->setText(1, QString::number(accelerate.y, 'g', 6));
        itemList.first()->child(0)->child(2)->setText(1, QString::number(accelerate.z, 'g', 6));
        itemList.first()->child(0)->child(3)->setText(1, QString::number(accelerate.temp, 'g', 6));

        itemList.first()->child(1)->child(0)->setText(1, QString::number(angularVelocity.x, 'g', 6));
        itemList.first()->child(1)->child(1)->setText(1, QString::number(angularVelocity.y, 'g', 6));
        itemList.first()->child(1)->child(2)->setText(1, QString::number(angularVelocity.z, 'g', 6));
        itemList.first()->child(1)->child(3)->setText(1, QString::number(angularVelocity.temp, 'g', 6));

        itemList.first()->child(2)->child(0)->setText(1, QString::number(angular.x, 'g', 6));
        itemList.first()->child(2)->child(1)->setText(1, QString::number(angular.y, 'g', 6));
        itemList.first()->child(2)->child(2)->setText(1, QString::number(angular.z, 'g', 6));
        itemList.first()->child(2)->child(3)->setText(1, QString::number(angular.temp, 'g', 6));

        itemList.first()->child(3)->child(0)->setText(1, QString::number(magneticField.x, 'g', 6));
        itemList.first()->child(3)->child(1)->setText(1, QString::number(magneticField.y, 'g', 6));
        itemList.first()->child(3)->child(2)->setText(1, QString::number(magneticField.z, 'g', 6));
        itemList.first()->child(3)->child(3)->setText(1, QString::number(magneticField.temp, 'g', 6));
    });

    /*
     * 伪彩色图标设置
     */
    connect(ui->checkBox_colorMap0_enable, &QCheckBox::stateChanged, this, [this](int state) {
        bool         status     = state == Qt::Unchecked ? false : true;
        QCustomPlot *customPlot = widget2CustomPlotList.at(0);
        customPlot->setVisible(status);
    });
    connect(ui->checkBox_colorMap1_enable, &QCheckBox::stateChanged, this, [this](int state) {
        bool         status     = state == Qt::Unchecked ? false : true;
        QCustomPlot *customPlot = widget2CustomPlotList.at(1);
        customPlot->setVisible(status);
    });
    connect(ui->checkBox_colorMap2_enable, &QCheckBox::stateChanged, this, [this](int state) {
        bool         status     = state == Qt::Unchecked ? false : true;
        QCustomPlot *customPlot = widget2CustomPlotList.at(2);
        customPlot->setVisible(status);
    });
    connect(ui->checkBox_colorMap3_enable, &QCheckBox::stateChanged, this, [this](int state) {
        bool         status     = state == Qt::Unchecked ? false : true;
        QCustomPlot *customPlot = widget2CustomPlotList.at(3);
        customPlot->setVisible(status);
    });

    connect(ui->btn_colorMap, &QPushButton::pressed, this, [this]() {
        int xMin = ui->lineEdit_colorMapXmin->text().toInt();
        int xMax = ui->lineEdit_colorMapXmax->text().toInt();
        int yMin = ui->lineEdit_colorMapYmin->text().toInt();
        int yMax = ui->lineEdit_colorMapYmax->text().toInt();
        int nx   = xMax - xMin;
        int ny   = yMax - yMin;
        int i    = 0;
        if(ui->comboBox_colorMap->currentText() == "通道0")
            i = 0;
        else if(ui->comboBox_colorMap->currentText() == "通道1")
            i = 1;
        else if(ui->comboBox_colorMap->currentText() == "通道2")
            i = 2;
        else if(ui->comboBox_colorMap->currentText() == "通道3")
            i = 3;

        QCPColorMap *colorMap = widget2QCPColorMapList.at(i);
        colorMap->data()->setSize(nx, ny);  // nx*ny(cells)
        colorMap->data()->setRange(QCPRange(xMin, xMax), QCPRange(yMin, yMax));

        QCustomPlot *customPlot = widget2CustomPlotList.at(i);

        colorMap->rescaleDataRange();
        customPlot->replot();
    });
}

void MainWindow::setToolBar()
{
    QVector<QAction *> act;
    act.append(new QAction("激光器设置", this));
    act.append(new QAction("电机设置", this));
    act.append(new QAction("高压设置", this));
    act.append(new QAction("预览设置", this));
    act.append(new QAction("显示侧边栏", this));
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    for(int i = 0; i < act.size(); i++)
    {
        ui->mainToolBar->addAction(act[i]);
    }

    connect(act[0], &QAction::triggered, this, [this]() { ui->tabWidget->setCurrentIndex(0); });
    connect(act[1], &QAction::triggered, this, [this]() { ui->tabWidget->setCurrentIndex(1); });
    connect(act[2], &QAction::triggered, this, [this]() { ui->tabWidget->setCurrentIndex(2); });
    connect(act[3], &QAction::triggered, this, [this]() { ui->tabWidget->setCurrentIndex(3); });
    connect(act[4], &QAction::triggered, this, [this]() {
        ui->dockWidget_left->show();
        ui->dockWidget_right->show();
    });
}

void MainWindow::plotLineSettings()
{
    //    ui->sampleDataPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    //ui->sampleDataPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->sampleDataPlot->legend->setVisible(true);  //右上角指示曲线的缩略框
    ui->sampleDataPlot->xAxis->setLabel(QStringLiteral("时间：ns"));
    ui->sampleDataPlot->yAxis->setLabel(QStringLiteral("AD采样值"));

    ui->sampleDataPlot->axisRect()->setRangeDrag(Qt::Horizontal);
    ui->sampleDataPlot->axisRect()->setRangeZoom(Qt::Horizontal);
    ui->sampleDataPlot->axisRect()->setRangeZoomAxes(ui->sampleDataPlot->xAxis, ui->sampleDataPlot->yAxis);
    ui->sampleDataPlot->setSelectionRectMode(QCP::srmZoom);

    QSharedPointer<QCPAxisTickerFixed> intTicker(new QCPAxisTickerFixed);
    //设置刻度之间的步长为1
    intTicker->setTickStep(1);
    //设置缩放策略
    intTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    //应用自定义整形ticker
    ui->sampleDataPlot->xAxis->setTicker(intTicker);

    for(int i = 0; i < 8; i++)
    {
        ui->sampleDataPlot->addGraph();
        ui->sampleDataPlot->graph(i)->setScatterStyle(QCPScatterStyle::ssDisc);
    }
    ui->sampleDataPlot->graph(0)->setPen(QPen(Qt::red));
    ui->sampleDataPlot->graph(0)->setName("通道0第一段");
    ui->sampleDataPlot->graph(1)->setPen(QPen(Qt::red));
    ui->sampleDataPlot->graph(1)->setName("通道0第二段");
    ui->sampleDataPlot->graph(2)->setPen(QPen(Qt::blue));
    ui->sampleDataPlot->graph(2)->setName("通道1第一段");
    ui->sampleDataPlot->graph(3)->setPen(QPen(Qt::blue));
    ui->sampleDataPlot->graph(3)->setName("通道1第二段");
    ui->sampleDataPlot->graph(4)->setPen(QPen(Qt::black));
    ui->sampleDataPlot->graph(4)->setName("通道2第一段");
    ui->sampleDataPlot->graph(5)->setPen(QPen(Qt::black));
    ui->sampleDataPlot->graph(5)->setName("通道2第二段");
    ui->sampleDataPlot->graph(6)->setPen(QPen(Qt::darkCyan));
    ui->sampleDataPlot->graph(6)->setName("通道3第一段");
    ui->sampleDataPlot->graph(7)->setPen(QPen(Qt::darkCyan));
    ui->sampleDataPlot->graph(7)->setName("通道3第二段");
}

void MainWindow::plotColormapSettings()
{
    QVBoxLayout *widget2VBox;
    widget2VBox = new QVBoxLayout;

    ui->widget2->setStyleSheet("QWidget#widget2{background-color:gray;}");
    ui->widget2->setLayout(widget2VBox);

    for(int i = 0; i < 4; ++i)
    {
        QCustomPlot *customPlot = new QCustomPlot;
        widget2CustomPlotList.append(customPlot);
        widget2VBox->addWidget(customPlot, 4);
    }

    for(int i = 0; i < 4; ++i)
    {
        // configure axis rect
        QCustomPlot *customPlot = widget2CustomPlotList.at(i);

        customPlot->setInteractions(QCP::Interaction::iRangeDrag |
                                    QCP::Interaction::iRangeZoom);
        customPlot->axisRect()->setupFullAxesBox(true);  //四刻度轴
        customPlot->xAxis->setLabel("电机角度(°)");
        customPlot->yAxis->setLabel("采样值");
        //if(i != 3)
        //customPlot->hide();

        QCPColorMap *colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
        widget2QCPColorMapList.append(colorMap);

        int nx = 180;
        int ny = 1024;

        colorMap->data()->setSize(nx, ny);                                 // nx*ny(cells)
        colorMap->data()->setRange(QCPRange(-90, 90), QCPRange(0, 1024));  // span the coordinate range
        colorMap->setDataScaleType(QCPAxis::ScaleType::stLinear);

        // add color scale:
        QCPColorScale *colorScale = new QCPColorScale(customPlot);
        customPlot->plotLayout()->addElement(0, 1, colorScale);  // add it to the right of the main axis rect
        colorScale->setType(QCPAxis::atRight);                   // scale shall be vertical bar with tick/axis labels right(default)
        colorMap->setColorScale(colorScale);
        colorScale->axis()->setLabel(QString("通道%1采样值").arg(i + 1));  // color scale name

        // set the color gradient of the color map to one of the presets:
        QCPColorGradient colorGradient;

        QMap<double, QColor> map;
        QList<QColor>        list;

        list.append(Qt::black);
        list.append(Qt::blue);
        list.append(Qt::darkGray);
        list.append(Qt::white);
        for(int i = 0; i < 4; ++i)
        {
            map[i * 0.15 + 0.3] = list.at(i % list.size());
        }
        colorGradient.setColorStops(map);

        colorGradient.loadPreset(QCPColorGradient::gpSpectrum);
        colorMap->setGradient(colorGradient);

        // make sure the axis rect and color scale synchronize their bottom and top margins.
        QCPMarginGroup *marginGroup = new QCPMarginGroup(customPlot);
        customPlot->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);

        // rescale the data dimension such that all data points in the span lie in the visualized by the color gradient
        colorMap->rescaleDataRange();

        // rescale the key and value axes so the whole color map is visible;
        customPlot->rescaleAxes();
    }
}

void MainWindow::updateColormap(QVector<WaveExtract::WaveformInfo> &allCh)
{
    QCustomPlot *    customPlot;
    QCPColorMap *    colorMap;
    QCPColorMapData *colorMapData;
    int              offset = 0;
    for(int i = 0; i < 4; ++i)
    {
        customPlot   = widget2CustomPlotList.at(i);
        colorMap     = widget2QCPColorMapList.at(i);
        colorMapData = colorMap->data();

        for(int keyIndex = 0; keyIndex < allCh[0].pos.length(); ++keyIndex)
        {
            if(allCh.size() == 8)
                offset = i * 2;

            int    key   = (int)allCh[offset].pos[keyIndex];
            double value = 0;
            if(i == 0)
            {
                value = 100;
                if(key == 200)
                    value = 200;
                if(key == 250)
                    value = 250;
                if(key == 300)
                    value = 300;
            }
            else
                value = allCh[offset].value[keyIndex];

            int frameN = (int)((allCh[offset].motorCnt / 163840.0) * 180);
            colorMapData->setCell(frameN, key, value);
        }

        colorMap->rescaleDataRange();
        customPlot->replot();
    }
}

void MainWindow::initSysInfoUi()
{
    ui->tableWidget_sysInfo->setColumnCount(2);
    ui->tableWidget_sysInfo->setRowCount(sysParaInfo.length());
    ui->tableWidget_sysInfo->setHorizontalHeaderLabels(QStringList() << "参数"
                                                                     << "值");
    ui->tableWidget_sysInfo->verticalHeader()->setVisible(false);  //隐藏列表头
        //    ui->tableWidget_sysInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);              //x先自适应宽度
    ui->tableWidget_sysInfo->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);  //然后设置要根据内容使用宽度的列

    for(int i = 0; i < sysParaInfo.length(); i++)
    {
        ui->tableWidget_sysInfo->setCellWidget(i, 0, new QLabel(sysParaInfo[i].name));
    }

    for(int i = 0; i < 10; i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setBackground(QBrush(QColor(Qt::lightGray)));
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
        ui->tableWidget_sysInfo->setItem(i, 0, item);
    }
}

void MainWindow::initFileListUi()
{
    //    ui->tableWidget_fileList->resizeColumnsToContents();
    ui->tableWidget_fileList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //然后设置要根据内容使用宽度的列
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    saveParameter();
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(timer1s == event->timerId())
    {
        // 判断sata状态机是否正常，从而得知文件结束信息有没有正确写入到硬盘
        if(sysStatus.udpLinkStatus & sysStatus.ssdLinkStatus & (!sysStatus.ssdStoreStatus))
        {
            if(sysParaInfo[8].value.toHex().toUInt() != 0x01)
            {
                ui->statusBar->showMessage("文件结束信息异常，没有写入硬盘", 0);
            }
        }

        if(ui->checkBox_autoReadSysInfo->isChecked())
            getSysInfo();
    }
}

void MainWindow::on_actionNote_triggered()
{
    NoteInfo *note = new NoteInfo;
    note->show();
}

void MainWindow::on_btnNorFlashRead_clicked()
{
    uint32_t addr;
    if(ui->rBtnDecAddr->isChecked())
    {
        addr = ui->lineEdit_NorFlashStartAddr->text().toInt(nullptr, 10);
    }
    else
    {
        addr = ui->lineEdit_NorFlashStartAddr->text().toInt(nullptr, 16);
    }
    ui->plain_NorDebugInfo->setPlainText(updateFlash->flashRead(addr));
}

void MainWindow::on_btnNorFlashErase_clicked()
{
    uint32_t addr;
    if(ui->rBtnDecAddr->isChecked())
    {
        addr = ui->lineEdit_NorFlashStartAddr->text().toInt(nullptr, 10);
    }
    else
    {
        addr = ui->lineEdit_NorFlashStartAddr->text().toInt(nullptr, 16);
    }
    updateFlash->flashErase(addr);
}

void MainWindow::on_btnNorFlasshReadFile_clicked()
{
    uint32_t startAddr, needNum;
    if(ui->rBtnDecAddr->isChecked())
    {
        startAddr = ui->lineEdit_NorFlashStartAddr->text().toInt(nullptr, 10);
        needNum   = ui->lineEdit_NorFlashReadLen->text().toInt(nullptr, 10) / 256;
    }
    else
    {
        startAddr = ui->lineEdit_NorFlashStartAddr->text().toInt(nullptr, 16);
        needNum   = ui->lineEdit_NorFlashReadLen->text().toInt(nullptr, 16) / 256;
    }

    ui->pBarNorFlashRead->setValue(0);
    ui->pBarNorFlashRead->setMaximum(needNum - 1);

    uint32_t   currentAddr;
    QByteArray ba;

    QFile file("origin.bin");
    file.open(QIODevice::ReadWrite);

    for(uint32_t i = 0; i < needNum; i++)
    {
        ui->pBarNorFlashRead->setValue(i);

        currentAddr     = startAddr + 256 * i;
        QByteArray data = updateFlash->flashRead(currentAddr);
        file.write(data);
    }
    file.close();
}

void MainWindow::on_bt_showWave_clicked()
{
    int total = ui->lineEdit_validFrameNum->text().toInt();
    if(total == 0)
        QMessageBox::warning(this, "warning", "没有有效数据");
    int start_index   = ui->spin_framePos->value();
    int interval_num  = ui->lineEdit_previewFrameInterval->text().toInt();
    int interval_time = ui->lineEdit_previewTimeInterval->text().toInt();

    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if(btn->objectName() == "bt_showWave")
        running = true;
    else
    {
        running = false;
    }
    for(int i = start_index; i < total; i += interval_num)
    {
        if(running)
        {
            ui->spin_framePos->setValue(i);
            QVector<quint8> sampleData = offlineWaveForm->getFrameData(i);  // 耗时小于1ms

            QVector<WaveExtract::WaveformInfo> allCh;
            if(WaveExtract::getWaveform(radarType, sampleData, allCh) == -1)  // 耗时小于1ms
            {
                QMessageBox::warning(this, "警告", "数据格式和当前雷达类型不匹配");
                return;
            }

            QByteArray convert;
            for(int i = 0; i < 88; i++)
                convert.append(sampleData[i]);
            gps->parserGpsData(convert);  //  耗时小于1ms

            for(int n = 0; n < allCh.size(); n++)
            {  //  耗时小于1ms
                ui->sampleDataPlot->graph(n)->setData(allCh[n].pos, allCh[n].value);
            }
            if(autoZoomPlot)
                ui->sampleDataPlot->rescaleAxes();
            ui->sampleDataPlot->replot();  // 耗时1-20ms，造成界面上的卡顿

            // ///////////
            updateColormap(allCh);

            if(interval_time == 0)
            {
                QCoreApplication::processEvents(QEventLoop::AllEvents);
                continue;
            }
            else
            {
                QEventLoop eventloop;
                QTimer::singleShot(interval_time, &eventloop, SLOT(quit()));
                eventloop.exec();
            }
        }
        else
        {
            return;
        }
    }
}

void MainWindow::getSysInfo()
{
    if(devInfo->getSysPara(sysParaInfo))
    {
        ui->statusBar->showMessage(tr("系统通信正常"), 0);
        for(int i = 0; i < sysParaInfo.length(); i++)
        {
            if(i == 0)  // 软件版本
                ui->tableWidget_sysInfo->setCellWidget(i, 1, new QLabel(sysParaInfo[i].value));
            else if(i == 5)  // 波形存储状态
            {
                ui->tableWidget_sysInfo->setCellWidget(i, 1, new QLabel(QString::number(sysParaInfo[i].value.toHex().toUInt(nullptr, 16))));
                qint32 value = BspConfig::ba2int(sysParaInfo[i].value);
                if(value == 1)
                {
                    sysStatus.ssdStoreStatus = true;
                    sysStatus.label_ssdStoreStatus->setText("存储状态：正在存储");
                }
                else
                {
                    sysStatus.ssdStoreStatus = false;
                    sysStatus.label_ssdStoreStatus->setText("存储状态：停止存储");
                }
            }
            else if(i == 6)  //  数据采集状态
            {
                if(sysParaInfo[i].value.contains(QByteArray(4, 0x01)))
                {
                    ui->tableWidget_sysInfo->setCellWidget(i, 1, new QLabel("正在采集"));
                    sysStatus.adCaptureStatus = true;
                    sysStatus.label_adCaptureStatus->setText("采集状态：正在采集");
                }
                else
                {
                    ui->tableWidget_sysInfo->setCellWidget(i, 1, new QLabel("停止采集"));
                    sysStatus.adCaptureStatus = false;
                    sysStatus.label_adCaptureStatus->setText("采集状态：停止采集");
                }
            }
            else if(i == 8)  // sata底层读写状态机
            {
                ui->tableWidget_sysInfo->setCellWidget(i, 1, new QLabel(QString::number(sysParaInfo[i].value.toHex().toUInt(nullptr, 16), 2)));
            }
            else if(i == 9)  // 文件读写状态机
            {
                quint16 value = sysParaInfo[i].value[1] * 256 + sysParaInfo[i].value[0];
                ui->tableWidget_sysInfo->setCellWidget(i, 1, new QLabel(QString::number(value, 2)));
            }
            else
                ui->tableWidget_sysInfo->setCellWidget(i, 1, new QLabel(QString::number(sysParaInfo[i].value.toHex().toUInt(nullptr, 16))));
        }
    }
    else
    {
        ui->statusBar->showMessage(tr("系统无法通信，检查网络连接"), 0);
    }
}

void MainWindow::showLaserInfo(LaserType4::LaserInfo &info)
{
    QList<QTreeWidgetItem *> itemList;

    itemList = ui->treeWidget_laser->findItems("电流(mA)", Qt::MatchExactly);
    itemList.first()->setText(1, QString::number(info.current * 10));

    itemList = ui->treeWidget_laser->findItems("激光头温度(°)", Qt::MatchExactly);
    itemList.first()->setText(1, QString::number(info.headTemp));

    itemList = ui->treeWidget_laser->findItems("LD温度(°)", Qt::MatchExactly);
    itemList.first()->setText(1, QString::number(info.ldTemp * 0.0001, 'f', 4));

    itemList = ui->treeWidget_laser->findItems("激光晶体温度(°)", Qt::MatchExactly);
    itemList.first()->setText(1, QString::number(info.laserCrystalTemp * 0.0001, 'f', 4));

    itemList = ui->treeWidget_laser->findItems("倍频晶体温度(°)", Qt::MatchExactly);
    itemList.first()->setText(1, QString::number(info.multiCrystalTemp * 0.0001, 'f', 4));

    itemList = ui->treeWidget_laser->findItems("状态位", Qt::MatchExactly);
    itemList.first()
        ->setText(1, QString("%1").arg(QString::number(info.statusBit, 2), 8, QLatin1Char('0')));

    itemList = ui->treeWidget_laser->findItems("错误位", Qt::MatchExactly);
    itemList.first()
        ->setText(1, QString("%1").arg(QString::number(info.errorBit, 2), 8, QLatin1Char('0')));
}

QString MainWindow::read_ip_address()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach(QHostAddress address, list)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            if(address.toString().contains("127.0."))
            {
                continue;
            }
            return address.toString();
        }
    }
    return 0;
}
