#include "radarwidget.h"
#include <qmessagebox.h>
#include <qsystemtrayicon.h>
#include <algorithm>
#include "bsp_config.h"
#include "ui_radarwidget.h"

#include "ui_navigation.h"

RadarWidget::RadarWidget(__radar_status__ para, QWidget *parent) :
    QWidget(parent),
    sysStatus(para),
    ui(new Ui::RadarWidget),
    thread(new QThread())
{
    ui->setupUi(this);
    uiConfig();

    // setWindowState(Qt::WindowMaximized);
    qRegisterMetaType<BspConfig::RadarType>("BspConfig::RadarType");
    qRegisterMetaType<WaveExtract::WaveformInfo>("WaveExtract::WaveformInfo");
    qRegisterMetaType<QVector<quint8>>("QVector<quint8>");
    qRegisterMetaType<QVector<WaveExtract::WaveformInfo>>("QVector<WaveformInfo>");
    qRegisterMetaType<BspConfig::Gps_Info>("BspConfig::Gps_Info");

    dispatch        = new ProtocolDispatch();
    preview         = new AdSampleControll();
    updateFlash     = new UpdateBin();
    offlineWaveForm = new OfflineWaveform();
    onlineWaveForm  = new OnlineWaveform();
    waveExtract     = new WaveExtract();

    daDriver = new DAControl();
    adDriver = new ADControl();
    devInfo  = new DevInfo();

    timer1s        = startTimer(1000);
    timerRefreshUI = startTimer(500);

    ssd = new ReadHardDisk();

    gps      = new GpsInfo();
    attitude = new AttitudeSensor;
    ms5837   = new MS5837;

    waterGuard.startSaveBase    = false;
    waterGuard.isSavedBase      = false;
    waterGuard.state            = WaveExtract::MOTOR_CNT_STATE::IDLE;
    waterGuard.videoMemoryDepth = 180;

    offlineWaveForm->moveToThread(thread);
    connect(this, SIGNAL(startPaserSampleNumber()), offlineWaveForm, SLOT(getADsampleNumber()));
    connect(offlineWaveForm, SIGNAL(finishSampleFrameNumber()), thread, SLOT(quit()));

    waveExtract->moveToThread(thread);
    thread->start();

    //    connect(waveShow, SIGNAL(finishSampleFrameNumber()), waveShow, SLOT(deleteLater()));
    //    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    initParameter();

    udpBind();
    initSignalSlot();

    plotLineSettings();
    plotColormapSettings();
    devInfo->getSysPara(sysParaInfo);
    initFileListUi();
    getSysInfo();
}

RadarWidget::~RadarWidget()
{
    delete ui;
}

void RadarWidget::initParameter()
{
    switch(sysStatus.radarType)
    {
        case 0:
            sysStatus.radarType = BspConfig::RADAR_TYPE_OCEAN;
            laserDriver         = new LaserType1();
            break;
        case 1:
            sysStatus.radarType = BspConfig::RADAR_TYPE_LAND;
            laserDriver         = new LaserType2();
            break;
        case 2:
            sysStatus.radarType = BspConfig::RADAR_TYPE_760;
            break;
        case 3:
            sysStatus.radarType = BspConfig::RADAR_TYPE_DOUBLE_WAVE;
            laserDriver         = new LaserType5();
            break;
        case 4:
            sysStatus.radarType = BspConfig::RADAR_TYPE_DRONE;
            laserDriver         = new LaserType3();
            break;
        case 5:
            sysStatus.radarType = BspConfig::RADAR_TYPE_WATER_GUARD;
            laserDriver         = new LaserType4();
            break;
        case 6:
            sysStatus.radarType = BspConfig::RADAR_TYPE_SECOND_INSTITUDE;
            laserDriver         = new LaserType6();
            break;
        case 8:
            sysStatus.radarType = BspConfig::RADAR_TYPE_DALIAN;
            laserDriver         = new LaserType8();
            break;
        default:
            sysStatus.radarType = BspConfig::RADAR_TYPE_DALIAN;
            laserDriver         = new LaserType3();
            break;
    }

    if(sysStatus.radarType == BspConfig::RADAR_TYPE_DOUBLE_WAVE)
        motorController = new PusiController();
    else if(sysStatus.radarType == BspConfig::RADAR_TYPE_DALIAN)
        motorController = new Elmo();
    else
        motorController = new EPOS2();

    ui->spinBox_sampleLen->setValue(sysStatus.previewSettings.sampleLen);
    ui->spinBox_sampleRate->setValue(sysStatus.previewSettings.sampleRatio);
    ui->spinBox_firstStartPos->setValue(sysStatus.previewSettings.firstPos);
    ui->spinBox_firstLen->setValue(sysStatus.previewSettings.firstLen);
    ui->spinBox_secondStartPos->setValue(sysStatus.previewSettings.secondPos);
    ui->spinBox_secondLen->setValue(sysStatus.previewSettings.secondLen);
    ui->spinBox_sumThreshold->setValue(sysStatus.previewSettings.sumThreshold);
    ui->spinBox_valueThreshold->setValue(sysStatus.previewSettings.valueThreshold);
    // ui->lineEdit_compressLen->setText(QString::number(sysStatus.previewSettings.compressLen));
    // ui->lineEdit_compressRatio->setText(QString::number(sysStatus.previewSettings.compressRatio));
}

void RadarWidget::saveParameter()
{
    return;
}

void RadarWidget::uiConfig()
{
    ui->tableWidget_fileList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_fileList->setContextMenuPolicy(Qt::ActionsContextMenu);  //设置为action菜单模式
    m_pActionCopy = new QAction(tr("读取当前文件"), ui->tableWidget_fileList);
    // ui->tableWidget_fileList->addAction(m_pActionCopy);
    ui->progressBar_extractHardDiskData->hide();

    ui->groupBox_tempVolt->hide();
    ui->treeWidget_attitude->expandAll();
    ui->treeWidget_attitude->resizeColumnToContents(0);
    QList<QTreeWidgetItem *> itemList;
    itemList = ui->treeWidget_attitude->findItems("姿态传感器", Qt::MatchExactly);
    itemList.first()->setHidden(true);
    itemList = ui->treeWidget_attitude->findItems("深度传感器", Qt::MatchExactly);
    itemList.first()->setHidden(true);

    QRegExp           decReg("[0-9]+$");
    QRegExpValidator *decValidator = new QRegExpValidator(decReg, this);
    QRegExp           floatReg("[0-9\.]+$");
    QRegExpValidator *floatValidator = new QRegExpValidator(floatReg, this);
    QRegExp           hexReg("[0-9,a-f,A-F]+$");
    QRegExpValidator *hexValidator = new QRegExpValidator(hexReg, this);

    ui->lineEdit_compressLen->setValidator(decValidator);
    ui->lineEdit_compressRatio->setValidator(decValidator);
    ui->lineEdit_pmtDelayTime->setValidator(decValidator);
    ui->lineEdit_pmtGateTime->setValidator(decValidator);

    ui->label_compressLen->hide();
    ui->lineEdit_compressLen->hide();
    ui->label_compressRatio->hide();
    ui->lineEdit_compressRatio->hide();
    ui->label_pmtGateTime->hide();
    ui->label_pmtDelayTime->hide();
    ui->lineEdit_pmtDelayTime->hide();
    ui->lineEdit_pmtGateTime->hide();
    ui->label_laserPower->hide();
    ui->label_laserBlueCurrent->hide();
    ui->spinBox_laserBlueCurrent->hide();

    ui->comboBox_laserPower->hide();
    ui->label_sampleDelay->hide();
    ui->lineEdit_sampleDelay->hide();

    ui->tabWidget->setTabEnabled(4, false);
    ui->tabWidget->setTabEnabled(5, false);
    ui->groupBox_norFlashTest->hide();
    ui->tabWidget->setCurrentIndex(0);

    ui->tabWidget_main->setTabEnabled(1, false);
    ui->tabWidget_main->setTabEnabled(2, false);
    ui->tabWidget_main->setCurrentIndex(0);

    ui->toolBox_motor->setItemEnabled(2, false);
    ui->rbtn_GLH->setVisible(false);
    ui->rbtn_POLARIZATION->setVisible(false);
    ui->groupBox_12->setVisible(false);
    ui->groupBox_7->setVisible(false);

    if(sysStatus.radarType == BspConfig::RADAR_TYPE_760)
    {
        ui->lineEdit_radarType->setText("760雷达");
        ui->label_secondStartPos->hide();
        ui->label_secondLen->hide();
        ui->label_subThreshold->hide();
        ui->label_sumThreshold->hide();
        ui->spinBox_secondStartPos->hide();
        ui->spinBox_secondLen->hide();
        ui->spinBox_valueThreshold->hide();
        ui->spinBox_sumThreshold->hide();
    }
    else if(sysStatus.radarType == BspConfig::RADAR_TYPE_DOUBLE_WAVE)
    {
        ui->lineEdit_radarType->setText("双波长雷达");

        ui->label_triggerMode->hide();
        ui->rbtn_triggerInside->hide();
        ui->rbtn_triggerOutside->hide();

        ui->comboBox_laserFreq->addItem("100");
        ui->comboBox_laserFreq->addItem("200");

        ui->label_laserGreenCurrent->setText("绿光(532)电流:A");

        ui->doubleSpinBox_laserGreenCurrent->setToolTip("0A <= current <=7.2A");
        ui->doubleSpinBox_laserGreenCurrent->setRange(0, 7.2);

        ui->label_laserBlueCurrent->show();
        ui->label_laserBlueCurrent->setText("蓝光(486)电流:A");

        ui->spinBox_laserBlueCurrent->show();
        ui->spinBox_laserBlueCurrent->setToolTip("0A <= current <=110A");
        ui->spinBox_laserBlueCurrent->setValue(70);
        ui->spinBox_laserBlueCurrent->setRange(0, 110);

        QStringList DA1List{"PMT_532_1", "PMT_486_1", "PMT_532_2", "PMT_486_2"};
        QStringList AD1List{"APD_TEMP", "APDHV_FB", "PMT_486_1", "PMT_532_2", "PMT_486_2", "PMT_532_1"};
        ui->comboBox_DAChSelect->addItems(DA1List);
        ui->comboBox_DAChSelect->setCurrentIndex(1);
        ui->comboBox_ADChSelect->addItems(AD1List);

        ui->toolBox_motor->setItemEnabled(1, false);

        ui->rbtn_GLH->setVisible(true);
        ui->rbtn_POLARIZATION->setVisible(true);

        doubleWaveConfig.sampleCnt  = 0;
        doubleWaveConfig.rescale[0] = false;
        doubleWaveConfig.rescale[1] = false;
        doubleWaveConfig.rescale[2] = false;
        doubleWaveConfig.rescale[3] = false;

        connect(ui->rbtn_GLH, &QRadioButton::clicked, this, [this]()
                {
            QMessageBox::information(NULL, "消息", "请注意是否需要进行电机归零操作");
            ui->lineEdit_motorTargetPosition->setToolTip("mrad(3.57°-36°)");

            ui->lineEdit_motorTargetPosition->setText("10");
            doubleWaveConfig.prev_angle     = 3.57;
            doubleWaveConfig.step_ratio     = 3631;
            doubleWaveConfig.min_view_angle = 3.57;
            doubleWaveConfig.max_view_angle = 36;
        });

        connect(ui->rbtn_POLARIZATION, &QRadioButton::clicked, this, [this]()
                {
            QMessageBox::information(NULL, "消息", "请注意是否需要进行电机归零操作");
            ui->lineEdit_motorTargetPosition->setToolTip("mrad(15°-110°)");
            ui->lineEdit_motorTargetPosition->setText("30");
            doubleWaveConfig.prev_angle     = 15;
            doubleWaveConfig.step_ratio     = 1239.5;
            doubleWaveConfig.min_view_angle = 15;
            doubleWaveConfig.max_view_angle = 110;
        });
    }
    else if(sysStatus.radarType == BspConfig::RADAR_TYPE_OCEAN)
    {
        ui->lineEdit_radarType->setText("海洋雷达");
        ui->label_laserGreenCurrent->hide();
        ui->doubleSpinBox_laserGreenCurrent->hide();
        ui->comboBox_laserFreq->addItem("5000");
        ui->comboBox_laserFreq->addItem("10000");

        ui->label_laserPower->show();
        ui->comboBox_laserPower->show();
        ui->btn_laserSetCurrent->setText("设置功率");

        QStringList DA1List{"APDHV", "PMT1HV", "PMT2HV", "PMT3HV"};
        QStringList AD1List{"APD TEMP", "APDHV FB", "PMT1HV FB", "PMT2HV FB", "PMT3HV FB"};
        ui->comboBox_DAChSelect->addItems(DA1List);
        ui->comboBox_DAChSelect->setCurrentIndex(0);
        ui->comboBox_ADChSelect->addItems(AD1List);
        ui->doubleSpinBox_DAValue->setRange(0, 250);

        ui->groupBox_tempVolt->show();
        ui->tabWidget->setTabEnabled(5, true);
        //        ui->label
    }
    else if(sysStatus.radarType == BspConfig::RADAR_TYPE_LAND)
    {
        ui->lineEdit_radarType->setText("陆地雷达");
        ui->label_triggerMode->hide();
        ui->rbtn_triggerInside->hide();
        ui->rbtn_triggerOutside->hide();
        ui->comboBox_laserFreq->addItem("100000");
        ui->comboBox_laserFreq->addItem("200000");
        ui->comboBox_laserFreq->addItem("400000");

        QStringList DA1List{"APDHV"};
        QStringList AD1List{"APD TEMP", "APDHV FB"};
        ui->comboBox_DAChSelect->addItems(DA1List);
        ui->comboBox_ADChSelect->addItems(AD1List);

        ui->doubleSpinBox_laserGreenCurrent->setValue(1000);
        ui->doubleSpinBox_laserGreenCurrent->setRange(0, 5000);

        ui->label_secondLen->hide();
        ui->spinBox_secondLen->hide();

        timerRefreshUI = startTimer(20);
    }
    else if(sysStatus.radarType == BspConfig::RADAR_TYPE_DRONE)
    {
        ui->lineEdit_radarType->setText("无人机雷达");
        ui->label_laserPower->hide();
        ui->comboBox_laserPower->hide();
        ui->comboBox_laserFreq->addItem("4000");

        ui->doubleSpinBox_laserGreenCurrent->setToolTip("3500 <= current <=4500");
        ui->doubleSpinBox_laserGreenCurrent->setRange(0, 1000);

        QStringList DA1List{"APDHV", "PMT1HV", "PMT2HV", "PMT3HV"};
        QStringList AD1List{"APD TEMP", "APDHV FB", "PMT1HV FB", "PMT2HV FB", "PMT3HV FB"};
        ui->comboBox_DAChSelect->addItems(DA1List);
        ui->comboBox_ADChSelect->addItems(AD1List);

        // ui->label_laserGreenCurrent->hide();
        // ui->doubleSpinBox_laserGreenCurrent->hide();
        // ui->btn_laserSetCurrent->hide();
        ui->doubleSpinBox_laserGreenCurrent->setRange(0, 6600);
        ui->doubleSpinBox_laserGreenCurrent->setValue(4100);

        ui->tabWidget->setTabEnabled(5, true);
    }
    else if(sysStatus.radarType == BspConfig::RADAR_TYPE_WATER_GUARD)
    {
        ui->lineEdit_radarType->setText("水下预警雷达");

        ui->comboBox_laserFreq->addItem("2000");
        ui->comboBox_laserFreq->addItem("3000");
        ui->comboBox_laserFreq->addItem("4000");
        ui->comboBox_laserFreq->addItem("5000");

        QStringList DA1List{"APDHV", "PMT1HV", "PMT2HV", "PMT3HV"};
        QStringList AD1List{"APD TEMP", "APDHV FB", "PMT1HV FB", "PMT2HV FB", "PMT3HV FB"};
        ui->comboBox_DAChSelect->addItems(DA1List);
        ui->comboBox_ADChSelect->addItems(AD1List);

        QList<QTreeWidgetItem *> itemList;
        itemList = ui->treeWidget_attitude->findItems("姿态传感器", Qt::MatchExactly);
        itemList.first()->setHidden(false);

        ui->doubleSpinBox_laserGreenCurrent->setRange(0, 6000);
        ui->doubleSpinBox_laserGreenCurrent->setValue(4000);

        ui->spinBox_firstLen->setEnabled(false);
        ui->spinBox_secondStartPos->setEnabled(false);
        ui->spinBox_secondLen->setEnabled(false);
        ui->spinBox_sumThreshold->setEnabled(false);
        ui->spinBox_valueThreshold->setEnabled(false);

        ui->label_pmtGateTime->show();
        ui->label_pmtDelayTime->show();
        ui->lineEdit_pmtDelayTime->show();
        ui->lineEdit_pmtGateTime->show();

        // ui->tabWidget_main->setTabEnabled(0, false);
        ui->tabWidget_main->setTabEnabled(1, true);
        ui->tabWidget_main->setCurrentIndex(1);
    }
    else if(sysStatus.radarType == BspConfig::RADAR_TYPE_SECOND_INSTITUDE)
    {
        ui->lineEdit_radarType->setText("海二所雷达");
        QStringList DA1List{"APDHV", "PMT1HV", "PMT2HV", "PMT3HV"};
        QStringList AD1List{"APD TEMP", "APDHV FB", "PMT1HV FB", "PMT2HV FB", "PMT3HV FB"};
        ui->comboBox_DAChSelect->addItems(DA1List);
        ui->comboBox_ADChSelect->addItems(AD1List);

        ui->comboBox_laserFreq->addItem("10");
        ui->label_laserGreenCurrent->setText("激光电流(A)");
        ui->doubleSpinBox_laserGreenCurrent->setValue(200);
        ui->spinBox_sampleRate->setValue(10);
    }
    else if(sysStatus.radarType == BspConfig::RADAR_TYPE_DALIAN)
    {
        ui->lineEdit_radarType->setText("大连雷达");

        ui->comboBox_laserFreq->addItem("5000");

        QStringList DA1List{"APDHV", "PMT1HV", "PMT2HV", "PMT3HV"};
        QStringList AD1List{"APD TEMP", "APDHV FB", "PMT1HV FB", "PMT2HV FB", "PMT3HV FB"};
        ui->comboBox_DAChSelect->addItems(DA1List);
        ui->comboBox_ADChSelect->addItems(AD1List);

        QList<QTreeWidgetItem *> itemList;
        itemList = ui->treeWidget_attitude->findItems("姿态传感器", Qt::MatchExactly);
        itemList.first()->setHidden(false);
        itemList = ui->treeWidget_attitude->findItems("深度传感器", Qt::MatchExactly);
        itemList.first()->setHidden(false);

        ui->doubleSpinBox_laserGreenCurrent->setRange(0, 6000);
        ui->doubleSpinBox_laserGreenCurrent->setValue(4000);

        ui->label_pmtGateTime->show();
        ui->label_pmtDelayTime->show();
        ui->lineEdit_pmtDelayTime->show();
        ui->lineEdit_pmtGateTime->show();

        // ui->tabWidget_main->setTabEnabled(0, false);
        ui->tabWidget_main->setTabEnabled(1, true);
        ui->tabWidget_main->setCurrentIndex(1);
    }
    else
    {
    }
    ui->checkBox_autoZoom->setChecked(true);

    QList<QTreeWidgetItem *> topItems;
    QList<QTreeWidgetItem *> subItems;
    switch(sysStatus.radarType)
    {
        case BspConfig::RADAR_TYPE_LAND:
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "激光器状态"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "外触发频率(kHz)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "电流(mA)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "温度"));
            ui->treeWidget_laser->addTopLevelItems(topItems);

            ui->treeWidget_laser->resizeColumnToContents(0);
            ui->treeWidget_laser->expandAll();
            break;
        case BspConfig::RADAR_TYPE_OCEAN:
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "点1温度"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "点2温度"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "点3温度"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "点4温度"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "点5温度"));
            ui->treeWidget_laser->addTopLevelItems(topItems);

            ui->treeWidget_laser->resizeColumnToContents(0);
            ui->treeWidget_laser->expandAll();
            break;
        case BspConfig::RADAR_TYPE_DOUBLE_WAVE:
        case BspConfig::RADAR_TYPE_DRONE:
        case BspConfig::RADAR_TYPE_DALIAN:
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "电流设定值(mA)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "电流实际值(mA)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "外触发频率(Hz)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "内触发频率(Hz)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "工作时间(s)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "状态位"));
            topItems.last()->addChild(new QTreeWidgetItem(topItems.last(), QStringList() << "是否开启激光器"));
            topItems.last()->addChild(new QTreeWidgetItem(topItems.last(), QStringList() << "是否外触发"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "错误位"));
            topItems.last()->addChild(new QTreeWidgetItem(topItems.last(), QStringList() << "驱动板应答是否丢失"));
            topItems.last()->addChild(new QTreeWidgetItem(topItems.last(), QStringList() << "控制板应答是否丢失"));
            topItems.last()->addChild(new QTreeWidgetItem(topItems.last(), QStringList() << "是否过温"));
            topItems.last()->addChild(new QTreeWidgetItem(topItems.last(), QStringList() << "是否欠温"));
            topItems.last()->addChild(new QTreeWidgetItem(topItems.last(), QStringList() << "是否存在频率问题"));
            ui->treeWidget_laser->addTopLevelItems(topItems);

            ui->treeWidget_laser->resizeColumnToContents(0);
            ui->treeWidget_laser->expandAll();
            break;
        case BspConfig::RADAR_TYPE_WATER_GUARD:
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "电流设定值(mA)"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "电流实际值(mA)"));
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
        case BspConfig::RADAR_TYPE_SECOND_INSTITUDE:
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "开关"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "电流"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "温度"));
            topItems.append(new QTreeWidgetItem(ui->treeWidget_laser, QStringList() << "错误码"));
            ui->treeWidget_laser->addTopLevelItems(topItems);

            ui->treeWidget_laser->resizeColumnToContents(0);
            ui->treeWidget_laser->expandAll();
            break;
        default:
            break;
    }
}

void RadarWidget::udpBind()
{
    ui->label_softwareVer->setText(sysStatus.localIP);
    udpSocket               = new QUdpSocket(this);
    sysStatus.udpLinkStatus = udpSocket->bind(QHostAddress(sysStatus.localIP), localPort);
    udpSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 1024 * 1024 * 1);
}

void RadarWidget::processPendingDatagram()
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

void RadarWidget::initSignalSlot()
{
    // 处理udp接收到的数据
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagram()));

    // 发送已经打包好的数据
    connect(dispatch, &ProtocolDispatch::frameDataReady, this, [this](QByteArray frame)
            {
        udpSocket->writeDatagram(frame.data(), frame.size(), sysStatus.deviceIP, sysStatus.devicePort);
    });

    connect(dispatch, &ProtocolDispatch::errorDataReady, this, [this](QString &error) {});

    /*
     * 读取系统参数信息相关逻辑
     */
    connect(devInfo, &DevInfo::sendDataReady, dispatch, &ProtocolDispatch::encode);
    connect(dispatch, &ProtocolDispatch::infoDataReady, devInfo, &DevInfo::setNewData);
    connect(ui->btn_ReadSysInfo, &QPushButton::pressed, this, [this]()
            {
        getSysInfo();
    });

    /*
     * 波形预览相关逻辑
     */
    connect(ui->btn_updateUI_intervalTime, &QPushButton::pressed, this, [this]()
            {
        int time       = ui->spinBox_updateUI_intervalTime->value();
        timerRefreshUI = startTimer(time);
    });
    connect(preview, &AdSampleControll::sendDataReady, dispatch, &ProtocolDispatch::encode);

    connect(ui->btn_setPreviewPara, &QPushButton::pressed, this, [this]()
            {
        sysStatus.previewSettings.sampleLen   = ui->spinBox_sampleLen->value();
        sysStatus.previewSettings.sampleRatio = ui->spinBox_sampleRate->value();
        sysStatus.previewSettings.firstPos    = ui->spinBox_firstStartPos->value();
        sysStatus.previewSettings.firstLen    = ui->spinBox_firstLen->value();
        sysStatus.previewSettings.secondPos   = ui->spinBox_secondStartPos->value();
        if(sysStatus.radarType == BspConfig::RADAR_TYPE_LAND)
            sysStatus.previewSettings.secondLen = 0;
        else
            sysStatus.previewSettings.secondLen = ui->spinBox_secondLen->value();
        sysStatus.previewSettings.sumThreshold   = ui->spinBox_sumThreshold->value();
        sysStatus.previewSettings.valueThreshold = ui->spinBox_valueThreshold->value();
        int     sampleDelay                      = ui->lineEdit_sampleDelay->text().toInt();
        quint16 pmtDelayTime                     = ui->lineEdit_pmtDelayTime->text().toUInt();
        quint16 pmtGateTime                      = ui->lineEdit_pmtGateTime->text().toUInt();

        if(sysStatus.previewSettings.secondPos < sysStatus.previewSettings.firstPos + sysStatus.previewSettings.firstLen)
        {
            QMessageBox::critical(NULL, "参数配置错误", "第二段起始位置需要小于第一段起始位置+第一段采样长度");
            return;
        }

        if(sysStatus.previewSettings.secondPos + sysStatus.previewSettings.secondLen >= sysStatus.previewSettings.sampleLen)
        {
            QMessageBox::critical(NULL, "参数配置错误", "第二段起始位置+第二段采样长度需要小于总采样长度");
            return;
        }

        preview->setTotalSampleLen(sysStatus.previewSettings.sampleLen);
        preview->setPreviewRatio(sysStatus.previewSettings.sampleRatio);

        if(sysStatus.radarType == BspConfig::RADAR_TYPE_LAND)
        {
            switch(sysStatus.previewSettings.laserFreq)
            {
                case 100000:
                    if(sysStatus.previewSettings.sampleLen > 9000)
                        QMessageBox::warning(NULL, "警告", "激光器频率=100Khz时，采样长度不大于9000.");
                    break;
                case 200000:
                    if(sysStatus.previewSettings.sampleLen > 4500)
                        QMessageBox::warning(NULL, "警告", "激光器频率=200Khz时，采样长度不大于4500.");
                    break;
                case 400000:
                    if(sysStatus.previewSettings.sampleLen > 2000)
                        QMessageBox::warning(NULL, "警告", "激光器频率=200Khz时，采样长度不大于2000.");
                    break;
                default:
                    break;
            }

            preview->setAlgoAPos((sysStatus.previewSettings.firstPos >> 3) << 3);
            preview->setAlgoALen((sysStatus.previewSettings.firstLen >> 3) << 3);
            preview->setAlgoBPos((sysStatus.previewSettings.secondPos >> 3) << 3);
            preview->setAlgoBSumThre(sysStatus.previewSettings.sumThreshold);
            preview->setAlgoBValueThre(sysStatus.previewSettings.valueThreshold);
            return;
        }
        if(sysStatus.radarType == BspConfig::RADAR_TYPE_SECOND_INSTITUDE)
        {
            if(sysStatus.previewSettings.firstLen + sysStatus.previewSettings.secondLen < 400)
            {
                QMessageBox::warning(NULL, "警告", "第一段采样长度+第二段采样长度需要大于400");
                return;
            }
        }

        if(sysStatus.radarType == BspConfig::RADAR_TYPE_DOUBLE_WAVE)
        {
            if(sysStatus.previewSettings.firstLen + sysStatus.previewSettings.secondLen >= 3000)
                QMessageBox::warning(NULL, "警告", "两段采样长度之和尽量不要大于3000");
        }
        else
        {
            if(sysStatus.previewSettings.firstLen + sysStatus.previewSettings.secondLen >= 1000)
                QMessageBox::warning(NULL, "警告", "两段采样长度之和尽量不要大于1000");
        }

        preview->setFirstPos(sysStatus.previewSettings.firstPos);
        preview->setFirstLen(sysStatus.previewSettings.firstLen);
        preview->setSecondPos(sysStatus.previewSettings.secondPos);
        preview->setSecondLen(sysStatus.previewSettings.secondLen);
        preview->setSumThreshold(sysStatus.previewSettings.sumThreshold);
        preview->setValueThreshold(sysStatus.previewSettings.valueThreshold);

        if(sysStatus.radarType == BspConfig::RADAR_TYPE_WATER_GUARD)
        {
            preview->setPmtDelayAndGateTime(pmtDelayTime, pmtGateTime);
        }
        if(sysStatus.radarType == BspConfig::RADAR_TYPE_DOUBLE_WAVE)
        {
            preview->setSampleDelay((sampleDelay >> 3) << 3);
        }
    });

    connect(ui->btn_sampleEnable, &QPushButton::pressed, this, [this]()
            {
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

    connect(ui->rbtn_previewOutsideTrg, &QRadioButton::clicked, this, [this]()
            {
        sysStatus.previewSettings.laserFreq = ui->comboBox_laserFreq->currentText().toInt(nullptr);
        preview->setTrgMode(MasterSet::OUTSIDE_TRG);
    });

    connect(ui->rbtn_previewInsideTrg, &QRadioButton::clicked, this, [this]()
            {
        sysStatus.previewSettings.laserFreq = ui->comboBox_laserFreq->currentText().toInt(nullptr);
        laserDriver->setFreq(sysStatus.previewSettings.laserFreq);
        preview->setTrgMode(MasterSet::INSIDE_TRG);
    });

    // 水下预警雷达相关设置
    connect(ui->btn_baseCapture, &QPushButton::pressed, this, [this]()
            {
        waterGuard.startSaveBase = true;
    });

    connect(dispatch, &ProtocolDispatch::onlineDataReady, onlineWaveForm, &OnlineWaveform::setNewData);
    connect(onlineWaveForm, &OnlineWaveform::fullSampleDataReady, this, [this](QByteArray &data)
            {
        if(binFile.isOpen())
            binFile.write(data);
        testCnt += data.size();
        sampleData.clear();
        for(auto &i : data)  // 数据格式转换
            sampleData.append(i);
        emit sampleDataReady(sysStatus.radarType, sampleData);
    });
    connect(this, &RadarWidget::sampleDataReady, waveExtract, &WaveExtract::getWaveform);
    connect(waveExtract, &WaveExtract::formatedWaveReady, this, &RadarWidget::showSampleData);

    connect(ui->checkBox_saveDataToFile, &QCheckBox::stateChanged, this, [this](int state)
            {
        if(state == Qt::Checked)
        {
            QString fileName = sysStatus.namePrefix + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
            binFile.setFileName(fileName + ".bin");
            binFile.open(QIODevice::WriteOnly);
        }
        else
            binFile.close();
    });

    connect(this, &RadarWidget::sampleDataReady, this, [this](BspConfig::RadarType type, const QVector<quint8> &sampleData)
            {
        QByteArray frame_head;
        for(int i = 0; i < 88; i++)
            frame_head.append(sampleData[i]);
        gps->parserGpsData(frame_head);  //  耗时小于1ms

        fpgaRadarType  = frame_head[84];
        fpgaVersion[0] = 'v';
        fpgaVersion[1] = frame_head[85];
        fpgaVersion[2] = '.';
        fpgaVersion[3] = frame_head[86];
        fpgaVersion[4] = frame_head[87];
    });

    /*
     * 图表控制相关逻辑
     */
    connect(ui->checkBox_autoZoom, &QCheckBox::stateChanged, this, [this](int state)
            {
        if(state == Qt::Checked)
            autoZoomPlot = true;
        else
            autoZoomPlot = false;
    });
    connect(ui->checkBox_ch0Enable, &QCheckBox::stateChanged, this, [this](int state)
            {
        bool status = state == Qt::Unchecked ? false : true;
        ui->sampleDataPlot->graph(0)->setVisible(status);
        ui->sampleDataPlot->graph(1)->setVisible(status);
        ui->sampleDataPlot->replot();
    });
    connect(ui->checkBox_ch1Enable, &QCheckBox::stateChanged, this, [this](int state)
            {
        bool status = state == Qt::Unchecked ? false : true;
        ui->sampleDataPlot->graph(2)->setVisible(status);
        ui->sampleDataPlot->graph(3)->setVisible(status);
        ui->sampleDataPlot->replot();
    });
    connect(ui->checkBox_ch2Enable, &QCheckBox::stateChanged, this, [this](int state)
            {
        bool status = state == Qt::Unchecked ? false : true;
        ui->sampleDataPlot->graph(4)->setVisible(status);
        ui->sampleDataPlot->graph(5)->setVisible(status);
        ui->sampleDataPlot->replot();
    });
    connect(ui->checkBox_ch3Enable, &QCheckBox::stateChanged, this, [this](int state)
            {
        bool status = state == Qt::Unchecked ? false : true;
        ui->sampleDataPlot->graph(6)->setVisible(status);
        ui->sampleDataPlot->graph(7)->setVisible(status);
        ui->sampleDataPlot->replot();
    });

    connect(ui->btn_axisRangeSet, &QPushButton::pressed, this, [this]()
            {
        ui->checkBox_autoZoom->setChecked(false);
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
    connect(ui->btn_selectOfflineFile, &QPushButton::pressed, this, [this]()
            {
        QString showFileName = QFileDialog::getOpenFileName(this, tr(""), "", tr("*.bin"));  //选择路径
        if(showFileName.size() == 0)
            return;
        ui->lineEdit_selectShowFile->setText(showFileName);
        offlineWaveForm->setWaveFile(showFileName);
        emit startPaserSampleNumber();
    });

    connect(offlineWaveForm, &OfflineWaveform::sendSampleFrameNumber, this, [this](qint32 number)
            {
        ui->spinBox_validFrameNum->setValue(number);
    });

    connect(offlineWaveForm, &OfflineWaveform::sendSampleFrameNumber, this, [this](qint32 number)
            {
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
    connect(ui->btn_showMotorCnt, &QPushButton::pressed, this, [this]()
            {
        QVector<double> motorCnt = offlineWaveForm->getMotorCnt();
        QVector<double> x;
        for(int i = 0; i < motorCnt.size(); i++)
            x.append(i);

        ui->sampleDataPlot->graph(0)->setData(x, motorCnt);
        ui->sampleDataPlot->rescaleAxes();
        ui->sampleDataPlot->replot();
    });

    connect(ui->btn_showGpsSubTime, &QPushButton::pressed, this, [this]()
            {
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
    connect(updateFlash, &UpdateBin::flashCommandReadySet, dispatch, &ProtocolDispatch::encode);
    connect(ui->btn_norFlashWrite, &QPushButton::pressed, this, [this]()
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

        QByteArray data;
        for(int i = 0; i < 256; i++)
        {
            data.append(i);
        }
        ui->btn_norFlashWrite->setEnabled(false);
        updateFlash->flashErase(addr);

        updateFlash->pageWrite(addr, data);
        ui->btn_norFlashWrite->setEnabled(true);
    });

    connect(ui->btn_selectUpdateFile, &QPushButton::pressed, this, [this]()
            {
        QString updateFilePath = QFileDialog::getOpenFileName(this, tr(""), "", tr("*"));  //选择路径
        if(updateFilePath.size() == 0)
            return;
        ui->lineEdit_updateFilePath->setText(updateFilePath);
    });

    connect(ui->btn_startUpdate, &QPushButton::pressed, this, [this]()
            {
        QString updateFilePath = ui->lineEdit_updateFilePath->text();
        if(updateFilePath.isEmpty())
        {
            QMessageBox::warning(this, "warning", "请先选择文件");
            return;
        }
        if(!updateFlash->checkBinFormat(updateFilePath))
        {
            QMessageBox::warning(this, "warning", "文件格式错误，请重新选择文件");
            return;
        }
        ui->pBar_updateBin->setValue(0);
        ui->pBar_updateBin->setMaximum(QFile(updateFilePath).size());
        ui->btn_startUpdate->setEnabled(false);

        if(updateFlash->flashUpdate(updateFilePath))
            QMessageBox::information(this, "information", "本次升级成功，断电重启后生效");
        else
            QMessageBox::warning(this, "warning", "本次升级失败，请重新尝试（不要断电）");

        ui->btn_startUpdate->setEnabled(true);
    });

    connect(updateFlash, &UpdateBin::updatedBytes, this, [this](qint32 bytes)
            {
        ui->pBar_updateBin->setValue(bytes);
    });

    connect(ui->btn_norFlashRead, &QPushButton::clicked, this, [this]()
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
        QByteArray recv = updateFlash->pageRead(addr);
        ui->plain_NorDebugInfo->appendPlainText(recv.toHex());
    });

    connect(ui->btn_norFlashReadFile, &QPushButton::clicked, this, [this]()
            {
        uint32_t startAddr, sectorNum;
        if(ui->rBtnDecAddr->isChecked())
        {
            startAddr = ui->lineEdit_NorFlashStartAddr->text().toInt(nullptr, 10);
            sectorNum = ui->lineEdit_NorFlashReadLen->text().toInt(nullptr, 10) / 256;
        }
        else
        {
            startAddr = ui->lineEdit_NorFlashStartAddr->text().toInt(nullptr, 16);
            sectorNum = ui->lineEdit_NorFlashReadLen->text().toInt(nullptr, 16) / 256;
        }

        ui->pBarNorFlashRead->setValue(0);
        ui->pBarNorFlashRead->setMaximum(sectorNum - 1);
        ui->btn_norFlashReadFile->setEnabled(false);

        uint32_t   currentAddr;
        QByteArray ba;

        QString fileName = QString("addr_0x%1.bin").arg(QString::number(startAddr, 16));

        QFile file(fileName);
        file.open(QIODevice::ReadWrite);

        QByteArray errorResult(256, 0xee);
        for(uint32_t i = 0; i < sectorNum; i++)
        {
            ui->pBarNorFlashRead->setValue(i);

            currentAddr     = startAddr + 256 * i;
            QByteArray data = updateFlash->pageReadWithCheck(currentAddr);
            if(data == errorResult)
            {
                qDebug() << "error addr at: " << currentAddr;
            }

            if(ui->checkBox_norFlashBitSwap->isChecked())
            {
                for(auto &i : data)
                    i = Common::bitSwap(i);
            }
            file.write(data);
        }
        file.close();

        ui->btn_norFlashReadFile->setEnabled(true);
    });

    /*
     * 激光器相关处理
     */

    if(sysStatus.radarType == BspConfig::RADAR_TYPE_DOUBLE_WAVE ||
       sysStatus.radarType == BspConfig::RADAR_TYPE_WATER_GUARD ||
       sysStatus.radarType == BspConfig::RADAR_TYPE_LAND ||
       sysStatus.radarType == BspConfig::RADAR_TYPE_OCEAN ||
       sysStatus.radarType == BspConfig::RADAR_TYPE_DALIAN ||
       sysStatus.radarType == BspConfig::RADAR_TYPE_SECOND_INSTITUDE ||
       sysStatus.radarType == BspConfig::RADAR_TYPE_DRONE)
    {
        connect(laserDriver, &LaserController::sendDataReady, dispatch, &ProtocolDispatch::encode);
        connect(dispatch, &ProtocolDispatch::laserDataReady, laserDriver, &LaserController::setNewData);
        connect(laserDriver, &LaserController::laserInfoReady, this, &RadarWidget::showLaserInfo);
    }

    connect(ui->comboBox_laserFreq, &QComboBox::currentTextChanged, this, [this](const QString &text)
            {
        if(text == "100000")
            ui->spinBox_sampleLen->setValue(9000);
        else if(text == "200000")
            ui->spinBox_sampleLen->setValue(4500);
        else if(text == "400000")
            ui->spinBox_sampleLen->setValue(2000);
    });

    connect(ui->btn_laserOpen, &QPushButton::pressed, this, [this]()
            {
        bool                      status = false;
        LaserController::OpenMode mode;
        if(ui->rbtn_triggerOutside->isChecked())
            mode = LaserController::OUT_SIDE;
        else
            mode = LaserController::IN_SIDE;
        status = laserDriver->setMode(mode);
        if(status == false)
            QMessageBox::warning(this, "警告", "指令流程异常，请尝试重新发送");

        sysStatus.previewSettings.laserFreq = ui->comboBox_laserFreq->currentText().toInt(nullptr);
        switch(sysStatus.radarType)
        {
            case BspConfig::RADAR_TYPE_LAND:
            case BspConfig::RADAR_TYPE_OCEAN:
            case BspConfig::RADAR_TYPE_DRONE:
            case BspConfig::RADAR_TYPE_DOUBLE_WAVE:
            case BspConfig::RADAR_TYPE_WATER_GUARD:
            case BspConfig::RADAR_TYPE_DALIAN:
                laserDriver->setFreq(ui->comboBox_laserFreq->currentText().toInt(nullptr));
                status = laserDriver->open();
                break;
            case BspConfig::RADAR_TYPE_SECOND_INSTITUDE:
                ui->btn_laserOpen->setEnabled(false);
                status = laserDriver->open();
                ui->btn_laserOpen->setEnabled(true);
                break;
            default:
                break;
        }
        if(!status)
            QMessageBox::warning(this, "警告", "指令流程异常，请尝试重新发送");
    });

    connect(ui->btn_laserClose, &QPushButton::pressed, this, [this]()
            {
        bool status = false;
        status      = laserDriver->close();
        if(!status)
            QMessageBox::warning(this, "警告", "指令流程异常，请尝试重新发送");
    });

    connect(ui->btn_laserReset, &QPushButton::pressed, this, [this]()
            {
        int status;
        if(laserDriver == nullptr)
        {
            QMessageBox::information(this, "消息", "激光器不支持此功能");
            return;
        }

        switch(status)
        {
            case -3:
                QMessageBox::warning(this, "警告", "激光器不支持此功能");
            case -2:
                QMessageBox::warning(this, "警告", "未实现此功能");
                break;
            case -1:
                QMessageBox::warning(this, "警告", "激光器通信异常");
                break;
            case 0:
                break;
            default:
                QMessageBox::warning(this, "警告", "指令响应异常");
                break;
        }
    });

    connect(ui->btn_laserSetCurrent, &QPushButton::pressed, this, [this]()
            {
        bool    status = false;
        QString power;
        switch(sysStatus.radarType)
        {
            case BspConfig::RADAR_TYPE_LAND:
                status                               = laserDriver->setCurrent(static_cast<int>(ui->doubleSpinBox_laserGreenCurrent->value()));
                status                               = laserDriver->setCurrent(static_cast<int>(ui->doubleSpinBox_laserGreenCurrent->value()));
                sysStatus.previewSettings.laserPower = static_cast<int>(ui->doubleSpinBox_laserGreenCurrent->value());
                break;
            case BspConfig::RADAR_TYPE_OCEAN:
                power                                = ui->comboBox_laserPower->currentText();
                sysStatus.previewSettings.laserPower = ui->comboBox_laserPower->currentText().toFloat();
                if(power == "1.8")
                    status = laserDriver->setPower(0);
                else if(power == "4.5")
                    status = laserDriver->setPower(1);
                else if(power == "7.6")
                    status = laserDriver->setPower(2);

                break;

            case BspConfig::RADAR_TYPE_DRONE:
            case BspConfig::RADAR_TYPE_DALIAN:
                // 上位机界面单位mA， 设置1000mA(0x3e8), 设置下去的值：100, 单位是0.01A，结果还是1000mA
                status = laserDriver->setCurrent(static_cast<int>(ui->doubleSpinBox_laserGreenCurrent->value()) / 10);
                break;
            case BspConfig::RADAR_TYPE_DOUBLE_WAVE:
                laserDriver->setFreq(ui->comboBox_laserFreq->currentText().toInt(nullptr));
                status = laserDriver->setCurrent(ui->doubleSpinBox_laserGreenCurrent->value() * 100);
                status = laserDriver->setPower(ui->spinBox_laserBlueCurrent->value() * 100);
                break;
            case BspConfig::RADAR_TYPE_WATER_GUARD:
                status = laserDriver->setPower(static_cast<int>(ui->doubleSpinBox_laserGreenCurrent->value()) / 10);
                break;
            case BspConfig::RADAR_TYPE_SECOND_INSTITUDE:
                status = laserDriver->setPower(static_cast<int>(ui->doubleSpinBox_laserGreenCurrent->value()));
                break;
            default:
                break;
        }
        if(status == false)
            QMessageBox::warning(this, "警告", "指令流程异常，请尝试重新发送");
    });

    connect(ui->rbtn_triggerInside, &QRadioButton::clicked, this, [this]()
            {
        bool status = false;
        status      = laserDriver->setMode(LaserController::IN_SIDE);
        if(status == false)
            QMessageBox::warning(this, "警告", "指令流程异常，请尝试重新发送");
    });

    connect(ui->rbtn_triggerOutside, &QRadioButton::clicked, this, [this]()
            {
        bool status = false;
        status      = laserDriver->setMode(LaserController::OUT_SIDE);
        if(status == false)
            QMessageBox::warning(this, "警告", "指令流程异常，请尝试重新发送");
    });

    connect(ui->btn_laserReadInfo, &QPushButton::pressed, this, [this]()
            {
        QList<QTreeWidgetItem *> itemList;
        switch(sysStatus.radarType)
        {
            case BspConfig::RADAR_TYPE_LAND:
            case BspConfig::RADAR_TYPE_OCEAN:
            case BspConfig::RADAR_TYPE_DRONE:
            case BspConfig::RADAR_TYPE_DALIAN:
            case BspConfig::RADAR_TYPE_DOUBLE_WAVE:
                while(laserDriver->getStatus() != true)
                    ;
                break;
            case BspConfig::RADAR_TYPE_WATER_GUARD:
                break;
            case BspConfig::RADAR_TYPE_SECOND_INSTITUDE:
                //                laser6Driver->getInfo();
                break;
            default:
                break;
        }
    });

    /*
     * 电机相关逻辑
     */
    connect(motorController, SIGNAL(sendDataReady(qint32, qint32, QByteArray &)), dispatch, SLOT(encode(qint32, qint32, QByteArray &)));
    connect(dispatch, &ProtocolDispatch::motorDataReady, motorController, &MotorController::setNewData);

    connect(ui->btn_motorReadSpeed, &QPushButton::pressed, this, [this]()
            {
        qint32 speed = 0;
        speed        = motorController->getActualVelocity();
        ui->lineEdit_motorShowSpeed->setText(QString::number(speed, 10));
    });

    connect(ui->btn_motorReadPosition, &QPushButton::pressed, this, [this]()
            {
        qint32 position = 0;
        position        = motorController->getActualPosition();
        ui->lineEdit_motorShowPosition->setText(QString::number(position, 10));
    });

    connect(ui->btn_motorStart, &QPushButton::pressed, this, [this]()
            {
        quint16 speed                        = ui->spinBox_motorTargetSpeed->value();
        sysStatus.previewSettings.motorSpeed = speed;
        motorResponse(motorController->run(speed));
    });

    connect(ui->btn_motorInit, &QPushButton::pressed, this, [this]()
            {
        ui->btn_motorInit->setEnabled(false);
        motorResponse(motorController->init());
        ui->btn_motorInit->setEnabled(true);
        ui->toolBox_motor->setCurrentIndex(1);
    });

    connect(ui->btn_motorMoveHome, &QPushButton::pressed, this, [this]()
            {
        ui->btn_motorMoveHome->setEnabled(false);
        if(motorController->moveToHome())
            ui->label_motorInfo->setText("电机归零后需要重新初始化，才能正常转动");
        else
            QMessageBox::warning(this, "warning", "电机通信异常");
        ui->btn_motorMoveHome->setEnabled(true);
        ui->toolBox_motor->setCurrentIndex(0);
    });

    connect(ui->btn_motorMovePostion, &QPushButton::pressed, this, [this]()
            {
        if(sysStatus.radarType == BspConfig::RADAR_TYPE_DOUBLE_WAVE)
        {
            if(ui->rbtn_GLH->isChecked() == false && ui->rbtn_POLARIZATION->isChecked() == false)
            {
                QMessageBox::warning(this, "warning", "请先选择设备型号");
                return;
            }
            double position = ui->lineEdit_motorTargetPosition->text().toDouble();

            if(position < doubleWaveConfig.min_view_angle || position > doubleWaveConfig.max_view_angle)
            {
                QMessageBox::warning(this, "warning", "视场角范围设置错误，请重新设置");
                return;
            }
            ui->btn_motorMovePostion->setEnabled(false);
            if(position > doubleWaveConfig.prev_angle)
            {
                motorController->moveToPosition((position - doubleWaveConfig.prev_angle) * doubleWaveConfig.step_ratio, 1);
            }
            else
            {
                motorController->moveToPosition((doubleWaveConfig.prev_angle - position) * doubleWaveConfig.step_ratio, 0);
            }
            doubleWaveConfig.prev_angle = position;
            ui->btn_motorMovePostion->setEnabled(true);
        }
        else
        {
            quint32 position = ui->lineEdit_motorTargetPosition->text().toUInt();
            if(position > 163840)
            {
                QMessageBox::warning(this, "warning", "电机位置不能大于163840");
                return;
            }
            motorController->moveToPosition(position, 1);
        }
    });
    connect(ui->btn_motorSweep, &QPushButton::pressed, this, [this]()
            {
        QMessageBox::warning(this, "warning", "未实现此功能");
    });

    /*
     * 采集数据保存相关逻辑
     */
    connect(ssd, SIGNAL(sendDataReady(qint32, qint32, QByteArray &)), dispatch, SLOT(encode(qint32, qint32, QByteArray &)));
    connect(dispatch, &ProtocolDispatch::ssdDataReady, ssd, &ReadHardDisk::setNewData);
    connect(ui->btn_ssdSearchSpace, &QPushButton::pressed, this, [this]()
            {
        if(sysStatus.ssdStoreStatus)
        {
            QMessageBox::warning(this, "warning", "不能在写文件时检索数据");
            return;
        }
        ui->btn_ssdSearchSpace->setEnabled(false);
        ui->tabWidget_main->setCurrentIndex(3);

        ui->tableWidget_fileList->clearContents();
        ui->tableWidget_fileList->setRowCount(1);

        ReadHardDisk::ValidFileInfo fileInfo;
        quint32                     startUnit = 0;

        ssd->inquireSpace(startUnit, fileInfo);

        switch(fileInfo.status)
        {
            case ReadHardDisk::FileStatus::FILE_INFO_FULL:
                ui->spinBox_ssdAvailFileUnit->setValue(fileInfo.fileUnit + 2);
                ui->spinBox_ssdAvailDataUnit->setValue(fileInfo.endUnit + 1);
                break;
            case ReadHardDisk::FileStatus::FILE_POSITION_ERROR:
                QMessageBox::warning(this, "warning", "上次文件信息写入出错, 请断电更换硬盘！");
                break;
            case ReadHardDisk::FileStatus::FILE_INFO_NONE:
                QMessageBox::warning(this, "warning", "没有读取到硬盘数据，确认网络连接正常");
                break;
        }

        ui->btn_ssdSearchSpace->setEnabled(true);
    });

    // 显示已经查询到的文件信息
    connect(ssd, &ReadHardDisk::fileDataReady, this, [this](ReadHardDisk::ValidFileInfo &fileInfo)
            {
        int row = ui->tableWidget_fileList->rowCount();

        quint32 fileSize = (fileInfo.endUnit - fileInfo.startUnit) * 16;
        QString size     = QString("%1GB / %2MB").arg(fileSize / 1024.0 / 1024).arg(fileSize / 1024.0);
        ui->tableWidget_fileList->setItem(row - 1, 0, new QTableWidgetItem(QString(fileInfo.name)));
        ui->tableWidget_fileList->setItem(row - 1, 1, new QTableWidgetItem(QString::number(fileInfo.startUnit)));
        ui->tableWidget_fileList->setItem(row - 1, 2, new QTableWidgetItem(QString::number(fileInfo.endUnit)));
        ui->tableWidget_fileList->setItem(row - 1, 3, new QTableWidgetItem(QString::number(fileInfo.startUnit, 16)));
        ui->tableWidget_fileList->setItem(row - 1, 4, new QTableWidgetItem(QString::number(fileInfo.endUnit, 16)));
        ui->tableWidget_fileList->setItem(row - 1, 5, new QTableWidgetItem(size));

        ui->tableWidget_fileList->setRowCount(row + 1);
        ui->tableWidget_fileList->resizeColumnsToContents();
    });

    connect(m_pActionCopy, &QAction::triggered, this, [this]
            {
        QMessageBox message(QMessageBox::NoIcon, "读取文件", "确定读取", QMessageBox::Yes | QMessageBox::No, NULL);
        if(message.exec() == QMessageBox::No)
            return;

        QModelIndexList indexes = ui->tableWidget_fileList->selectionModel()->selectedIndexes();

        QString filePath;
        filePath = QFileDialog::getExistingDirectory();  //选择路径
        if(filePath.size() == 0)
            return;
        filePath += "/";

        QString fileName = ui->tableWidget_fileList->item(indexes[0].row(), indexes[0].column())->text();
        filePath += fileName;
        filePath += ".bin";

        QFile file(filePath);
        file.open(QIODevice::WriteOnly);

        uint32_t startAddr = ui->tableWidget_fileList->item(indexes[1].row(), indexes[1].column())->text().toUInt(nullptr, 10);
        uint32_t stopAddr  = ui->tableWidget_fileList->item(indexes[2].row(), indexes[2].column())->text().toUInt(nullptr, 10);

        ui->progressBar_extractHardDiskData->setRange(startAddr, stopAddr - 1);
        ui->progressBar_extractHardDiskData->setValue(startAddr);
        ui->tableWidget_fileList->setContextMenuPolicy(Qt::NoContextMenu);  //设置为action菜单模式

        QVector<QByteArray> data;
        QElapsedTimer       timer;
        timer.start();

        for(uint32_t addr = startAddr; addr < stopAddr; addr++)
        //        for(uint32_t addr = startAddr; addr < startAddr + 1; addr++)
        {
            QByteArray array;
            data = ssd->readDiskUnit(addr);

            for(int i = 0; i < data.size(); i++)
                array.append(data[i]);
            file.write(array);

            ui->progressBar_extractHardDiskData->setValue(addr);
            ui->label_extractFileTime->setText("消耗时间:" + QString::number(timer.elapsed() / 1000) + "s");
        }

        file.close();
        ui->tableWidget_fileList->setContextMenuPolicy(Qt::ActionsContextMenu);  //设置为action菜单模式
    });

    connect(ui->btn_extractFileByNum, &QPushButton::pressed, this, [this]()
            {
        int row    = ui->tableWidget_fileList->rowCount();
        int number = ui->spinBox_extractFileNum->value();
        if(number > row)
            return;

        QString filePath;
        filePath = QFileDialog::getExistingDirectory();  //选择路径
        if(filePath.size() == 0)
            return;
        filePath += "/";

        QTableWidgetItem *item     = ui->tableWidget_fileList->item(number - 1, 0);
        QString           fileName = item->text();
        filePath += fileName;
        filePath += ".bin";

        QFile file(filePath);
        file.open(QIODevice::WriteOnly);

        item               = ui->tableWidget_fileList->item(number - 1, 1);
        uint32_t startAddr = item->text().toUInt(nullptr, 10);
        item               = ui->tableWidget_fileList->item(number - 1, 2);
        uint32_t stopAddr  = item->text().toUInt(nullptr, 10);

        ui->progressBar_extractHardDiskData->setRange(startAddr, stopAddr - 1);
        ui->progressBar_extractHardDiskData->setValue(startAddr);
        ui->btn_extractFileByNum->setEnabled(false);

        QVector<QByteArray> data;
        QElapsedTimer       timer;
        timer.start();

        for(uint32_t addr = startAddr; addr < stopAddr; addr++)
        //        for(uint32_t addr = startAddr; addr < startAddr + 1; addr++)
        {
            QByteArray array;
            data = ssd->readDiskUnit(addr);

            for(int i = 0; i < data.size(); i++)
                array.append(data[i]);
            file.write(array);

            ui->progressBar_extractHardDiskData->setValue(addr);
            ui->label_extractFileTime->setText("消耗时间:" + QString::number(timer.elapsed() / 1000) + "s");
        }

        file.close();
        ui->btn_extractFileByNum->setEnabled(true);
    });

    connect(ui->btn_extractFileByAddr, &QPushButton::pressed, this, [this]()
            {
        if(ui->spinBox_extractFileStopAddr->value() <= ui->spinBox_extractFileStartAddr->value())
            return;
        if(ui->lineEdit_extractFileName->text().length() == 0)
            return;

        QString filePath;
        filePath = QFileDialog::getExistingDirectory();  //选择路径
        if(filePath.size() == 0)
            return;
        filePath += "/";

        QString fileName = ui->lineEdit_extractFileName->text();
        filePath += fileName;

        QFile file(filePath);
        file.open(QIODevice::WriteOnly);

        uint32_t startAddr = ui->spinBox_extractFileStartAddr->value();
        uint32_t stopAddr  = ui->spinBox_extractFileStopAddr->value();

        ui->progressBar_extractHardDiskData->setRange(startAddr, stopAddr - 1);
        ui->progressBar_extractHardDiskData->setValue(startAddr);
        ui->btn_extractFileByNum->setEnabled(false);
        QVector<QByteArray> data;
        QElapsedTimer       timer;
        timer.start();

        for(uint32_t addr = startAddr; addr < stopAddr; addr++)
        {
            QByteArray array;
            data = ssd->readDiskUnit(addr);

            if(data.size() == 0)
            {
                QMessageBox::warning(this, "warning", "没有读取到硬盘数据, 读取结束");
                ui->btn_extractFileByNum->setEnabled(true);
                file.close();
                return;
            }

            for(int i = 0; i < data.size(); i++)
                array.append(data[i]);
            file.write(array);

            ui->progressBar_extractHardDiskData->setValue(addr);
            ui->label_extractFileTime->setText("消耗时间:" + QString::number(timer.elapsed() / 1000) + "s");
        }

        file.close();
        ui->btn_extractFileByNum->setEnabled(true);
    });

    connect(ui->btn_ssdEnableStore, &QPushButton::pressed, this, [this]()
            {
        if(!sysStatus.adCaptureStatus)
        {
            QMessageBox::warning(this, "warning", "请先开始采集");
            return;
        }

        ui->btn_ssdEnableStore->setEnabled(false);

        quint32 fileUnit = ui->spinBox_ssdAvailFileUnit->value();

        QString fileName = sysStatus.namePrefix + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");

        if(ui->lineEdit_ssdStoreFileName->text().length() != 0)
            fileName.append(ui->lineEdit_ssdStoreFileName->text());
        ssd->setSaveFileName(fileUnit, fileName);

        quint32 dataUnit = ui->spinBox_ssdAvailDataUnit->value();
        ssd->setSaveFileAddr(dataUnit);
        ssd->enableStoreFile(0x01);

        // #ifdef DEBUG_WATER_GUARD
        testCnt = 0;
        elapsedTimer.start();
        qDebug() << "start: testCnt = " << testCnt;
        // #endif
    });

    connect(ui->btn_ssdDisableStore, &QPushButton::pressed, this, [this]()
            {
        QMessageBox message(QMessageBox::NoIcon, "停止存储", "真的要停止存储吗", QMessageBox::Yes | QMessageBox::No, NULL);
        if(message.exec() == QMessageBox::No)
            return;
        ui->btn_ssdEnableStore->setEnabled(true);
        ssd->enableStoreFile(0x00);
        //
        // #ifdef DEBUG_WATER_GUARD
        qDebug() << "stop: testCnt = " << testCnt;
        int t = elapsedTimer.elapsed();
        if(sysStatus.previewSettings.laserFreq > 0)
        {
            double   period_s     = 1.0 / sysStatus.previewSettings.laserFreq;
            uint64_t sample_count = (t / 1000.0) / period_s;

            int      bytes_of_sample = 128 + 8 * sysStatus.previewSettings.firstLen + 4 * (sysStatus.previewSettings.secondLen * 2 + 4);
            uint64_t bytes_of_total  = sample_count * bytes_of_sample;
            // int totalBytes = (t / 1000.0) * (sysStatus.previewSettings.laserFreq / sysStatus.previewSettings.sampleRatio) * (128 + sysStatus.previewSettings.firstLen * 8);

            QString info = QString("存储数据时间(ms): %1\n采样波形次数:%2\n单次采样数据量(byte): %3\n理论全部存储数据量(bytes): %4")
                               .arg(t)
                               .arg(sample_count)
                               .arg(bytes_of_sample)
                               .arg(bytes_of_total);
            QMessageBox::information(NULL, "information", info);
        }
        else
        {
            QMessageBox::information(this, "information", "请先设置激光器采样频率");
        }
        // #endif
    });

    /*
     * DA设置相关逻辑
     */
    connect(daDriver, SIGNAL(sendDataReady(qint32, qint32, QByteArray &)), dispatch, SLOT(encode(qint32, qint32, QByteArray &)));
    connect(ui->btn_DASetValue, &QPushButton::pressed, this, [this]()
            {
        quint32 chNum       = ui->comboBox_DAChSelect->currentIndex();
        double  analogValue = ui->doubleSpinBox_DAValue->value();

        switch(ui->comboBox_DAChSelect->currentIndex())
        {
            case 0:
                sysStatus.previewSettings.APDHV = analogValue;
                break;
            case 1:
                sysStatus.previewSettings.PMT1HV = analogValue;
                break;
            case 2:
                sysStatus.previewSettings.PMT2HV = analogValue;
                break;
            case 3:
                sysStatus.previewSettings.PMT3HV = analogValue;
                break;
            default:
                break;
        }
        qint32 digitValue = 0;
        switch(sysStatus.radarType)
        {
            case BspConfig::RADAR_TYPE_LAND:
                digitValue = static_cast<quint32>((analogValue - 3.434) / 0.017);
                break;
            case BspConfig::RADAR_TYPE_DOUBLE_WAVE:
                digitValue = static_cast<qint32>((analogValue + 0.007) / 0.001);
                if(chNum == 0)
                    chNum += 4;
                break;
            case BspConfig::RADAR_TYPE_OCEAN:
            case BspConfig::RADAR_TYPE_DRONE:
            case BspConfig::RADAR_TYPE_WATER_GUARD:
            case BspConfig::RADAR_TYPE_SECOND_INSTITUDE:
            case BspConfig::RADAR_TYPE_DALIAN:
                if(chNum == 0)
                {
                    digitValue       = static_cast<qint32>((analogValue - 8.208) / 0.113);
                    QByteArray frame = BspConfig::int2ba(digitValue);
                    dispatch->encode(MasterSet::TEMP_VOLT_X1, 4, frame);
                }
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
        ui->plainTextEdit_DASetLog->appendPlainText(QDateTime::currentDateTime().toString("hh:mm:ss") +
                                                    "-> " +
                                                    ui->comboBox_DAChSelect->currentText() +
                                                    ": " + QString::number(analogValue, 'g', 5) +
                                                    "V");
    });

    connect(ui->btn_DAClearAll, &QPushButton::pressed, this, [this]()
            {
        for(int i = 0; i < 4; i++)
        {
            daDriver->setChannalValue(i, 0);
            ui->plainTextEdit_DASetLog->appendPlainText(QDateTime::currentDateTime().toString("hh:mm:ss") +
                                                        "-> " +
                                                        ui->comboBox_DAChSelect->itemText(i) +
                                                        ": 0V");
        }
    });

    /*
     * AD设置相关逻辑
     */
    connect(dispatch, &ProtocolDispatch::ADDataReady, adDriver, &ADControl::setNewData);
    connect(adDriver, &ADControl::sendDataReady, dispatch, &ProtocolDispatch::encode);
    connect(ui->btn_ADReadValue, &QPushButton::pressed, this, [this]()
            {
        quint32 chNum = ui->comboBox_ADChSelect->currentIndex();

        qint32 digitValue  = adDriver->getChannalValue(chNum);
        double analogValue = 0;
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
                case 5:
                    analogValue = (digitValue + 1.392) / 822.7;
                    break;
            }
            ui->lineEdit_ADValue->setText(QString::number(analogValue, 'g', 2));
        }
        ui->plainTextEdit_ADSetLog->appendPlainText(QDateTime::currentDateTime().toString("hh:mm:ss") +
                                                    "-> " +
                                                    ui->comboBox_ADChSelect->currentText() +
                                                    ": " +
                                                    ui->lineEdit_ADValue->text() + "V");
    });

    connect(ui->btn_ADReadAll, &QPushButton::pressed, this, [this]()
            {
        QMessageBox::warning(this, "warning", "还未实现此功能");
    });

    connect(ui->btn_setTempVolt, &QPushButton::pressed, this, [this]()
            {
        QByteArray frame = BspConfig::int2ba(ui->spinBox_temp_volt_x2->value());
        dispatch->encode(MasterSet::TEMP_VOLT_X2, 4, frame);
    });

    /*
     * gps信息处理
     */
    connect(dispatch, &ProtocolDispatch::gpsFrameReady, gps, &GpsInfo::parserGpsData);
    connect(gps, &GpsInfo::gpsDataReady, this, [this](BspConfig::Gps_Info &data)
            {
        emit                     sendGpsInfo(data);
        QList<QTreeWidgetItem *> itemList;

        itemList = ui->treeWidget_attitude->findItems("GPS信息", Qt::MatchExactly);
        itemList.first()->child(0)->setText(1, QString::number(data.week));
        itemList.first()->child(1)->setText(1, QString::number(data.current_week_ms, 'g', 10));
        itemList.first()->child(2)->setText(1, QString::number(data.sub_time / 10000000.0, 'g', 6));
        itemList.first()->child(3)->setText(1, QString::number(data.latitude, 'g', 6));
        itemList.first()->child(4)->setText(1, QString::number(data.longitude, 'g', 6));
        itemList.first()->child(5)->setText(1, QString::number(data.altitude, 'g', 6));
        itemList.first()->child(6)->setText(1, QString::number(data.roll, 'g', 6));
        itemList.first()->child(7)->setText(1, QString::number(data.pitch, 'g', 6));
        itemList.first()->child(8)->setText(1, QString::number(data.heading, 'g', 6));
    });

    connect(ui->btn_cameraEnable, &QPushButton::pressed, this, [this]()
            {
        uint32_t   second = ui->spinBox_cameraPeriod->value();
        QByteArray frame  = BspConfig::int2ba(second);
        dispatch->encode(MasterSet::CAMERA_FREQ_SET, 4, frame);
        frame = BspConfig::int2ba(0x01);
        dispatch->encode(MasterSet::CAMERA_ENABLE, 4, frame);
    });

    connect(ui->btn_cameraDisable, &QPushButton::pressed, this, [this]()
            {
        QByteArray frame = BspConfig::int2ba(0x00);
        dispatch->encode(MasterSet::CAMERA_ENABLE, 4, frame);
    });

    /*
     * 姿态传感器信息处理
     */
    connect(dispatch, &ProtocolDispatch::attitudeDataReady, attitude, &AttitudeSensor::parserFrame);
    connect(attitude, &AttitudeSensor::sendAttitudeResult, this, [this](AttitudeSensor::AttitudeInfo accelerate, AttitudeSensor::AttitudeInfo angularVelocity, AttitudeSensor::AttitudeInfo angular, AttitudeSensor::AttitudeInfo magneticField)
            {
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
        itemList.first()->child(2)->child(1)->setText(1, QString::number(angular.z, 'g', 6));
        itemList.first()->child(2)->child(2)->setText(1, QString::number(angular.y, 'g', 6));
        itemList.first()->child(2)->child(3)->setText(1, QString::number(angular.temp, 'g', 6));

        itemList.first()->child(3)->child(0)->setText(1, QString::number(magneticField.x, 'g', 6));
        itemList.first()->child(3)->child(1)->setText(1, QString::number(magneticField.y, 'g', 6));
        itemList.first()->child(3)->child(2)->setText(1, QString::number(magneticField.z, 'g', 6));
        itemList.first()->child(3)->child(3)->setText(1, QString::number(magneticField.temp, 'g', 6));
    });

    connect(dispatch, &ProtocolDispatch::ms5837DataReady, ms5837, &MS5837::parserFrame);
    connect(ms5837, &MS5837::sendMS5837Result, this, [this](MS5837::Info info)
            {
        QList<QTreeWidgetItem *> itemList;

        itemList = ui->treeWidget_attitude->findItems("深度传感器", Qt::MatchExactly);
        itemList.first()->child(0)->setText(1, QString::number(info.temperature, 'g', 6));
        itemList.first()->child(0)->setText(1, QString::number(info.depth, 'g', 6));
    });

    /*
     * 伪彩色图标设置
     */
    connect(ui->checkBox_colorMap0_enable, &QCheckBox::stateChanged, this, [this](int state)
            {
        bool         status     = state == Qt::Unchecked ? false : true;
        QCustomPlot *customPlot = widget2CustomPlotList.at(0);
        customPlot->setVisible(status);
    });
    connect(ui->checkBox_colorMap1_enable, &QCheckBox::stateChanged, this, [this](int state)
            {
        bool         status     = state == Qt::Unchecked ? false : true;
        QCustomPlot *customPlot = widget2CustomPlotList.at(1);
        customPlot->setVisible(status);
    });
    connect(ui->checkBox_colorMap2_enable, &QCheckBox::stateChanged, this, [this](int state)
            {
        bool         status     = state == Qt::Unchecked ? false : true;
        QCustomPlot *customPlot = widget2CustomPlotList.at(2);
        customPlot->setVisible(status);
    });
    connect(ui->checkBox_colorMap3_enable, &QCheckBox::stateChanged, this, [this](int state)
            {
        bool         status     = state == Qt::Unchecked ? false : true;
        QCustomPlot *customPlot = widget2CustomPlotList.at(3);
        customPlot->setVisible(status);
    });

    connect(ui->checkBox_colorMap0_rescale, &QCheckBox::stateChanged, this, [this](int state)
            {
        bool status                 = state == Qt::Unchecked ? false : true;
        doubleWaveConfig.rescale[0] = status;
        if(!status)
            widget2QCPColorMapList.at(0)->setDataRange(QCPRange(0, 1024));
    });
    connect(ui->checkBox_colorMap1_rescale, &QCheckBox::stateChanged, this, [this](int state)
            {
        bool status                 = state == Qt::Unchecked ? false : true;
        doubleWaveConfig.rescale[1] = status;
        if(!status)
            widget2QCPColorMapList.at(1)->setDataRange(QCPRange(0, 1024));
    });
    connect(ui->checkBox_colorMap2_rescale, &QCheckBox::stateChanged, this, [this](int state)
            {
        bool status                 = state == Qt::Unchecked ? false : true;
        doubleWaveConfig.rescale[2] = status;
        if(!status)
            widget2QCPColorMapList.at(2)->setDataRange(QCPRange(0, 1024));
    });
    connect(ui->checkBox_colorMap3_rescale, &QCheckBox::stateChanged, this, [this](int state)
            {
        bool status                 = state == Qt::Unchecked ? false : true;
        doubleWaveConfig.rescale[3] = status;
        if(!status)
            widget2QCPColorMapList.at(3)->setDataRange(QCPRange(0, 1024));
    });

    connect(ui->btn_colorMap, &QPushButton::pressed, this, [this]() {});

    connect(ui->comboBox_DAChSelect, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index)
            {
        switch(sysStatus.radarType)
        {
            case BspConfig::RADAR_TYPE_OCEAN:
                if(index == 0)
                    ui->doubleSpinBox_DAValue->setRange(0, 250);
                else
                    ui->doubleSpinBox_DAValue->setRange(0, 5);
                break;

            case BspConfig::RADAR_TYPE_LAND:
                if(index == 0)
                    ui->doubleSpinBox_DAValue->setRange(0, 60);
                break;
            default:
                break;
        }
    });

    /*
     *  参数保存相关逻辑
     */
    connect(ui->btn_parameterSave, &QPushButton::pressed, this, [this]()
            {
        QString configPath = sysStatus.namePrefix + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
        if(ui->lineEdit_parameterFileName->text().length() != 0)
            configPath.append(ui->lineEdit_parameterFileName->text());
        configUser = new QSettings("./data/" + configPath + ".ini", QSettings::IniFormat);

        sysStatus.previewSettings.laserFreq = ui->comboBox_laserFreq->currentText().toInt();
        if(sysStatus.radarType == BspConfig::RADAR_TYPE_LAND)
            sysStatus.previewSettings.laserPower = ui->doubleSpinBox_laserGreenCurrent->value();
        else
            sysStatus.previewSettings.laserPower = ui->comboBox_laserPower->currentText().toDouble();
        sysStatus.previewSettings.motorSpeed     = ui->spinBox_motorTargetSpeed->value();
        sysStatus.previewSettings.sampleLen      = ui->spinBox_sampleLen->value();
        sysStatus.previewSettings.sampleRatio    = ui->spinBox_sampleRate->value();
        sysStatus.previewSettings.firstPos       = ui->spinBox_firstStartPos->value();
        sysStatus.previewSettings.firstLen       = ui->spinBox_firstLen->value();
        sysStatus.previewSettings.secondPos      = ui->spinBox_secondStartPos->value();
        sysStatus.previewSettings.secondLen      = ui->spinBox_secondLen->value();
        sysStatus.previewSettings.sumThreshold   = ui->spinBox_sumThreshold->value();
        sysStatus.previewSettings.valueThreshold = ui->spinBox_valueThreshold->value();

        configUser->setValue("Laser/freq", sysStatus.previewSettings.laserFreq);
        configUser->setValue("Laser/power", sysStatus.previewSettings.laserPower);
        configUser->setValue("Motor/speed", sysStatus.previewSettings.motorSpeed);

        configUser->setValue("Voltage/APDHV", sysStatus.previewSettings.APDHV);
        configUser->setValue("Voltage/PMT1HV", sysStatus.previewSettings.PMT1HV);
        configUser->setValue("Voltage/PMT2HV", sysStatus.previewSettings.PMT2HV);
        configUser->setValue("Voltage/PMT3HV", sysStatus.previewSettings.PMT3HV);

        configUser->setValue("Preview/sampleLen", sysStatus.previewSettings.sampleLen);
        configUser->setValue("Preview/sampleRatio", sysStatus.previewSettings.sampleRatio);
        configUser->setValue("Preview/firstPos", sysStatus.previewSettings.firstPos);
        configUser->setValue("Preview/firstLen", sysStatus.previewSettings.firstLen);
        configUser->setValue("Preview/secondPos", sysStatus.previewSettings.secondPos);
        configUser->setValue("Preview/secondLen", sysStatus.previewSettings.secondLen);
        configUser->setValue("Preview/sumThreshold", sysStatus.previewSettings.sumThreshold);
        configUser->setValue("Preview/valueThreshold", sysStatus.previewSettings.valueThreshold);
    });

    connect(ui->btn_parameterLoad, &QPushButton::pressed, this, [this]()
            {
        QString configFile = QFileDialog::getOpenFileName(this, tr(""), "", tr("*.ini"));  //选择路径
        if(configFile.size() == 0)
            return;

        configUser                               = new QSettings(configFile, QSettings::IniFormat);
        sysStatus.previewSettings.laserFreq      = configUser->value("Laser/freq").toInt();
        sysStatus.previewSettings.laserPower     = configUser->value("Laser/power").toDouble();
        sysStatus.previewSettings.motorSpeed     = configUser->value("Motor/speed").toInt();
        sysStatus.previewSettings.APDHV          = configUser->value("Voltage/APDHV").toDouble();
        sysStatus.previewSettings.PMT1HV         = configUser->value("Voltage/PMT1HV").toDouble();
        sysStatus.previewSettings.PMT2HV         = configUser->value("Voltage/PMT2HV").toDouble();
        sysStatus.previewSettings.PMT3HV         = configUser->value("Voltage/PMT3HV").toDouble();
        sysStatus.previewSettings.sampleLen      = configUser->value("Preview/sampleLen").toInt();
        sysStatus.previewSettings.sampleRatio    = configUser->value("Preview/sampleRatio").toInt();
        sysStatus.previewSettings.firstPos       = configUser->value("Preview/firstPos").toInt();
        sysStatus.previewSettings.firstLen       = configUser->value("Preview/firstLen").toInt();
        sysStatus.previewSettings.secondPos      = configUser->value("Preview/secondPos").toInt();
        sysStatus.previewSettings.secondLen      = configUser->value("Preview/secondLen").toInt();
        sysStatus.previewSettings.sumThreshold   = configUser->value("Preview/sumThreshold").toInt();
        sysStatus.previewSettings.valueThreshold = configUser->value("Preview/valueThreshold").toInt();

        int idx;
        idx = ui->comboBox_laserFreq->findText(QString::number(sysStatus.previewSettings.laserFreq));
        if(idx < 0)
            QMessageBox::warning(this, "警告", "参数读取错误");
        ui->comboBox_laserFreq->setCurrentIndex(idx);

        if(sysStatus.radarType == BspConfig::RADAR_TYPE_LAND)
        {
            ui->doubleSpinBox_laserGreenCurrent->setValue(sysStatus.previewSettings.laserPower);
        }
        else
        {
            idx = ui->comboBox_laserPower->findText(QString::number(sysStatus.previewSettings.laserPower));
            if(idx < 0)
                QMessageBox::warning(this, "警告", "参数读取错误");
            ui->comboBox_laserPower->setCurrentIndex(idx);
        }
        ui->spinBox_motorTargetSpeed->setValue(sysStatus.previewSettings.motorSpeed);
        ui->spinBox_sampleLen->setValue(sysStatus.previewSettings.sampleLen);
        ui->spinBox_sampleRate->setValue(sysStatus.previewSettings.sampleRatio);
        ui->spinBox_firstStartPos->setValue(sysStatus.previewSettings.firstPos);
        ui->spinBox_firstLen->setValue(sysStatus.previewSettings.firstLen);
        ui->spinBox_secondStartPos->setValue(sysStatus.previewSettings.secondPos);
        ui->spinBox_secondLen->setValue(sysStatus.previewSettings.secondLen);
        ui->spinBox_sumThreshold->setValue(sysStatus.previewSettings.sumThreshold);
        ui->spinBox_valueThreshold->setValue(sysStatus.previewSettings.valueThreshold);

        QString temp = QString("上次保存的参数:\nAPDHV : %1 \nPMT1HV : %2\nPMT2HV : %3\nPMT3HV : %4\n请依次设置")
                           .arg(QString::number(sysStatus.previewSettings.APDHV, 'f', 2))
                           .arg(QString::number(sysStatus.previewSettings.PMT1HV, 'f', 2))
                           .arg(QString::number(sysStatus.previewSettings.PMT2HV, 'f', 2))
                           .arg(QString::number(sysStatus.previewSettings.PMT3HV, 'f', 2));

        ui->plainTextEdit_DASetLog->appendPlainText(temp);
    });
}

void RadarWidget::plotLineSettings()
{
    QSharedPointer<QCPAxisTickerFixed> intTicker(new QCPAxisTickerFixed);
    //设置刻度之间的步长为1
    intTicker->setTickStep(1);
    //设置缩放策略
    intTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);

    ui->sampleDataPlot->legend->setVisible(true);  //右上角指示曲线的缩略框
    ui->sampleDataPlot->xAxis->setLabel(QStringLiteral("时间：ns"));
    ui->sampleDataPlot->yAxis->setLabel(QStringLiteral("AD采样值"));

    ui->sampleDataPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);  // 可以按住鼠标拖动图标，鼠标滚轮缩放图表

    // 选择矩形区域缩放
    // ui->sampleDataPlot->axisRect()->setRangeDrag(Qt::Horizontal);
    // ui->sampleDataPlot->axisRect()->setRangeZoom(Qt::Horizontal);
    // ui->sampleDataPlot->axisRect()->setRangeZoomAxes(ui->sampleDataPlot->xAxis, ui->sampleDataPlot->yAxis);
    // ui->sampleDataPlot->setSelectionRectMode(QCP::srmZoom);

    //应用自定义整形ticker
    ui->sampleDataPlot->xAxis->setTicker(intTicker);

    for(int i = 0; i < 8; i++)
    {
        ui->sampleDataPlot->addGraph();
        // ui->sampleDataPlot->graph(i)->setScatterStyle(QCPScatterStyle::ssDisc);
    }
    ui->sampleDataPlot->graph(0)->setPen(QPen(Qt::red));
    ui->sampleDataPlot->graph(0)->setName("通道0第一段");
    ui->sampleDataPlot->graph(1)->setPen(QPen(Qt::red));
    ui->sampleDataPlot->graph(1)->setName("通道0第二段");
    ui->sampleDataPlot->graph(2)->setPen(QPen(Qt::blue));
    ui->sampleDataPlot->graph(2)->setName("通道1第一段");
    ui->sampleDataPlot->graph(3)->setPen(QPen(Qt::blue));
    ui->sampleDataPlot->graph(3)->setName("通道1第二段");
    ui->sampleDataPlot->graph(4)->setPen(QPen(Qt::green));
    ui->sampleDataPlot->graph(4)->setName("通道2第一段");
    ui->sampleDataPlot->graph(5)->setPen(QPen(Qt::green));
    ui->sampleDataPlot->graph(5)->setName("通道2第二段");
    ui->sampleDataPlot->graph(6)->setPen(QPen(Qt::black));
    ui->sampleDataPlot->graph(6)->setName("通道3第一段");
    ui->sampleDataPlot->graph(7)->setPen(QPen(Qt::black));
    ui->sampleDataPlot->graph(7)->setName("通道3第二段");
}

void RadarWidget::plotColormapSettings()
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
        if(sysStatus.radarType == BspConfig::RADAR_TYPE_DOUBLE_WAVE)
            customPlot->xAxis->setLabel("采样次数");
        else
            customPlot->xAxis->setLabel("电机角度(°)");
        customPlot->yAxis->setLabel("时间(ns)");
        // if(i != 3)
        // customPlot->hide();

        QCPColorMap *colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
        widget2QCPColorMapList.append(colorMap);

        int nx = colorMap_X_max;  // x轴，角度,
        int ny = 1500;            // y轴，采样数据的时间轴

        colorMap->data()->setSize(nx, ny);  // nx*ny(cells)

        if(sysStatus.radarType == BspConfig::RADAR_TYPE_DOUBLE_WAVE)
            colorMap->data()->setRange(QCPRange(0, colorMap_X_max), QCPRange(0, 1500));  // 显示这些点的绘图坐标范围
        else
            colorMap->data()->setRange(QCPRange(-90, 90), QCPRange(0, 500));  // span the coordinate range

        colorMap->setDataScaleType(QCPAxis::ScaleType::stLinear);

        // add color scale:
        QCPColorScale *colorScale = new QCPColorScale(customPlot);
        customPlot->plotLayout()->addElement(0, 1, colorScale);  // add it to the right of the main axis rect
        colorScale->setType(QCPAxis::atRight);                   // scale shall be vertical bar with tick/axis labels right(default)
        colorMap->setColorScale(colorScale);
        colorScale->setRangeDrag(false);
        colorScale->setRangeZoom(false);
        colorScale->axis()->setLabel(QString("通道%1采样值").arg(i + 1));  // color scale name

        // set the color gradient of the color map to one of the presets:
        QCPColorGradient colorGradient;
        colorGradient.loadPreset(QCPColorGradient::gpSpectrum);
        colorMap->setGradient(colorGradient);

        // make sure the axis rect and color scale synchronize their bottom and top margins.
        QCPMarginGroup *marginGroup = new QCPMarginGroup(customPlot);
        customPlot->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);

        // colorMap->rescaleDataRange();  // 使数据范围包含最小到最大的整个数据集, 二选一即可
        colorMap->setDataRange(QCPRange(0, 1024));  // 指定映射到渐变颜色的数据范围

        // rescale the key and value axes so the whole color map is visible;
        customPlot->rescaleAxes();
    }
}

/**
 * @brief mainwidget::updateColormap
 * @param chart, 用于指定在那个QCustomPlot上绘制
 * @param angle，伪彩色图的x轴，采样数据的电机角度值
 * @param key，伪彩色图的y轴, 是采样数据的时间序列
 * @param data，伪彩色图的z轴，颜色显示，是采样数据的值序列，和时间序列一一对应
 */
void RadarWidget::updateColormap(int chart, int angle, const QVector<double> &key, const QVector<double> &data)
{
    QCustomPlot     *customPlot;
    QCPColorMap     *colorMap;
    QCPColorMapData *colorMapData;

    customPlot   = widget2CustomPlotList.at(chart);
    colorMap     = widget2QCPColorMapList.at(chart);
    colorMapData = colorMap->data();

    int y_offset = key[0];
    customPlot->yAxis->setLabel("时间(ns), 偏移" + QString::number(y_offset));

    int x = angle;
    // 如果某次采样数据太长，是显示前面采样的数据
    for(int keyIndex = 0; keyIndex < key.length() && keyIndex < 1500; ++keyIndex)
    {
        // y轴数据的范围只能是[0,1500]，所以要减去偏移值
        int    y     = key[keyIndex] - y_offset;
        double value = data[keyIndex];

        colorMapData->setCell(x, y, value);
    }
    //    colorMap->rescaleDataRange();
    //    customPlot->replot();
}

void RadarWidget::initFileListUi()
{
    //    ui->tableWidget_fileList->resizeColumnsToContents();
    ui->tableWidget_fileList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //然后设置要根据内容使用宽度的列
}

void RadarWidget::motorResponse(MotorController::MOTOR_STATUS status)
{
    switch(status)
    {
        case MotorController::MOTOR_STATUS::SUCCESS:
            ui->label_motorInfo->setText("电机初始化已完成");
            break;
        case MotorController::MOTOR_STATUS::FAILED:
            QMessageBox::warning(this, "warning", "电机通信异常");
            break;
        case MotorController::MOTOR_STATUS::NO_FEATURE:
            QMessageBox::warning(this, "warning", "没有此功能");
            break;
        case MotorController::MOTOR_STATUS::NO_IMPLEMENT:
            QMessageBox::warning(this, "warning", "还没有实现此功能");
            break;
    }
}

void RadarWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    saveParameter();
}

void RadarWidget::timerEvent(QTimerEvent *event)
{
    if(timer1s == event->timerId())
    {
        // 判断sata状态机是否正常，从而得知文件结束信息有没有正确写入到硬盘
        if(sysStatus.udpLinkStatus & sysStatus.ssdLinkStatus & (!sysStatus.ssdStoreStatus))
        {
            if(sysParaInfo[8].value.toHex().toUInt() != 0x01)
            {
                // ui->statusBar->showMessage("文件结束信息异常，没有写入硬盘", 0);
            }
        }

        if(ui->checkBox_autoReadSysInfo->isChecked())
            getSysInfo();
        ui->label_fpgaVer->setText(fpgaVersion);

        if(fpgaRadarType != -1 && fpgaRadarType != sysStatus.radarType)
        {
            // ui->statusBar->showMessage("底层配置的雷达类型(" + QString::number(fpgaRadarType) + ")与上位机配置的雷达类型不一致", 0);
        }
    }
    if(timerRefreshUI == event->timerId())
    {
        refreshUIFlag = true;
    }
}

void RadarWidget::on_bt_showWave_clicked()
{
    int total = ui->spinBox_validFrameNum->value();
    if(total == 0)
        QMessageBox::warning(this, "warning", "没有有效数据");
    int start_index   = ui->spin_framePos->value();
    int interval_num  = ui->spinBox_previewFrameInterval->value();
    int interval_time = ui->spinBox_previewTimeInterval->value();

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
            sampleData = offlineWaveForm->getFrameData(i);  // 耗时小于1ms
            waveExtract->getWaveform(sysStatus.radarType, sampleData);

            QByteArray frame_head;
            for(int i = 0; i < 88; i++)
                frame_head.append(sampleData[i]);
            gps->parserGpsData(frame_head);  //  耗时小于1ms

            fpgaRadarType  = frame_head[84];
            fpgaVersion[0] = 'v';
            fpgaVersion[1] = frame_head[85];
            fpgaVersion[2] = '.';
            fpgaVersion[3] = frame_head[86];
            fpgaVersion[4] = frame_head[87];

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

void RadarWidget::getSysInfo()
{
    if(devInfo->getSysPara(sysParaInfo))
    {
        sysStatus.udpLinkStatus             = true;
        sysStatus.previewSettings.laserFreq = sysParaInfo[0].value.toHex().toUInt(nullptr, 16);

        if(devInfo->getRadarType() != sysStatus.radarType)
        {
            // ui->statusBar->showMessage("底层配置的雷达类型(" + QString::number(devInfo->getRadarType()) + ")与上位机配置的雷达类型不一致", 0);
        }
        fpgaVersion = devInfo->getFpgaVer().toUtf8();

        QList<QTreeWidgetItem *> itemList;
        itemList = ui->treeWidget_attitude->findItems("系统参数", Qt::MatchExactly);

        for(int i = 0; i < sysParaInfo.size(); i++)
        {
            if(i == 4)  // 波形存储状态
            {
                itemList.first()->child(i)->setText(1, QString::number(sysParaInfo[i].value.toHex().toUInt(nullptr, 16)));
                qint32 value = BspConfig::ba2int(sysParaInfo[i].value);
                if(value == 1)
                {
                    sysStatus.ssdStoreStatus = true;
                }
                else
                {
                    sysStatus.ssdStoreStatus = false;
                }
            }
            else if(i == 5)  //  数据采集状态
            {
                if(sysParaInfo[i].value.contains(QByteArray(4, 0x01)))
                {
                    itemList.first()->child(i)->setText(1, "正在采集");
                    sysStatus.adCaptureStatus = true;
                }
                else
                {
                    itemList.first()->child(i)->setText(1, "停止采集");
                    sysStatus.adCaptureStatus = false;
                }
            }
            else if(i == 7)  // sata底层读写状态机
            {
                int value        = sysParaInfo[i].value.toHex().toUInt(nullptr, 16);
                sysStatus.state1 = value;
                itemList.first()->child(i)->setText(1, QString::number(value, 2));
            }
            else if(i == 8)  // 文件读写状态机
            {
                quint16 value = sysParaInfo[i].value[1] * 256 + sysParaInfo[i].value[0];
                itemList.first()->child(i)->setText(1, QString::number(value, 2));
            }
            else
                itemList.first()->child(i)->setText(1, QString::number(sysParaInfo[i].value.toHex().toUInt(nullptr, 16)));
        }
    }
    else
    {
        sysStatus.udpLinkStatus = false;
        // ui->statusBar->setStyleSheet("color:red");
        // ui->statusBar->showMessage("系统无法通信，检查网络连接", 0);
    }
}

void RadarWidget::showLaserInfo(LaserType4::LaserInfo &info)
{
    QList<QTreeWidgetItem *> itemList;
    switch(sysStatus.radarType)
    {
        case BspConfig::RADAR_TYPE_LAND:

            itemList = ui->treeWidget_laser->findItems("激光器状态", Qt::MatchExactly);
            if(info.status == 0)
                itemList.first()->setText(1, "close");
            else
                itemList.first()->setText(1, "open");

            itemList = ui->treeWidget_laser->findItems("外触发频率(kHz)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.freq_outside));
            itemList = ui->treeWidget_laser->findItems("电流(mA)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.real_current));
            itemList = ui->treeWidget_laser->findItems("温度", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.temp, 'f', 4));
            break;
            break;
        case BspConfig::RADAR_TYPE_OCEAN:
            itemList = ui->treeWidget_laser->findItems("点1温度", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(static_cast<int>(info.temp)));
            itemList = ui->treeWidget_laser->findItems("点2温度", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(static_cast<int>(info.headTemp)));
            itemList = ui->treeWidget_laser->findItems("点3温度", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(static_cast<int>(info.ldTemp)));
            itemList = ui->treeWidget_laser->findItems("点4温度", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(static_cast<int>(info.laserCrystalTemp)));
            itemList = ui->treeWidget_laser->findItems("点5温度", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(static_cast<int>(info.multiCrystalTemp)));
            break;
        case BspConfig::RADAR_TYPE_DRONE:
        case BspConfig::RADAR_TYPE_DALIAN:
            itemList = ui->treeWidget_laser->findItems("电流设定值(mA)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.expected_current * 10));

            itemList = ui->treeWidget_laser->findItems("电流实际值(mA)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.real_current * 10));

            itemList = ui->treeWidget_laser->findItems("外触发频率(Hz)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.freq_outside));

            itemList = ui->treeWidget_laser->findItems("内触发频率(Hz)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.freq_inside));

            itemList = ui->treeWidget_laser->findItems("工作时间(s)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.work_time));

            itemList = ui->treeWidget_laser->findItems("状态位", Qt::MatchExactly);
            itemList.first()->setText(1, QString("%1").arg(QString::number(info.statusBit, 2), 8, QLatin1Char('0')));
            itemList.first()->child(0)->setText(1, QString::number((info.statusBit >> 0) & 0x01));
            itemList.first()->child(1)->setText(1, QString::number((info.statusBit >> 1) & 0x01));

            itemList = ui->treeWidget_laser->findItems("错误位", Qt::MatchExactly);
            itemList.first()->setText(1, QString("%1").arg(QString::number(info.errorBit, 2), 8, QLatin1Char('0')));
            for(int i = 0; i < 5; i++)
                itemList.first()->child(i)->setText(1, QString::number((info.errorBit >> i) & 0x01));
            break;
        case BspConfig::RADAR_TYPE_DOUBLE_WAVE:
            itemList = ui->treeWidget_laser->findItems("电流设定值(mA)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.expected_current * 10));

            itemList = ui->treeWidget_laser->findItems("电流实际值(mA)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.real_current * 10));

            itemList = ui->treeWidget_laser->findItems("外触发频率(Hz)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.freq_outside));

            itemList = ui->treeWidget_laser->findItems("内触发频率(Hz)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.freq_inside));

            itemList = ui->treeWidget_laser->findItems("工作时间(s)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.work_time));

            itemList = ui->treeWidget_laser->findItems("状态位", Qt::MatchExactly);
            itemList.first()->setText(1, QString("%1").arg(QString::number(info.statusBit, 2), 8, QLatin1Char('0')));
            itemList.first()->child(0)->setText(1, QString::number((info.statusBit >> 0) & 0x01));
            itemList.first()->child(1)->setText(1, QString::number((info.statusBit >> 1) & 0x01));

            itemList = ui->treeWidget_laser->findItems("错误位", Qt::MatchExactly);
            itemList.first()->setText(1, QString("%1").arg(QString::number(info.errorBit, 2), 8, QLatin1Char('0')));
            for(int i = 0; i < 5; i++)
                itemList.first()->child(i)->setText(1, QString::number((info.errorBit >> i) & 0x01));
            break;
        case BspConfig::RADAR_TYPE_WATER_GUARD:

            itemList = ui->treeWidget_laser->findItems("电流设定值(mA)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.expected_current * 10));

            itemList = ui->treeWidget_laser->findItems("电流实际值(mA)", Qt::MatchExactly);
            itemList.first()->setText(1, QString::number(info.real_current * 10));

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
            break;
        case BspConfig::RADAR_TYPE_SECOND_INSTITUDE:

            itemList = ui->treeWidget_laser->findItems("开关", Qt::MatchExactly);
            //            itemList.first()->setText(1, info.status);

            itemList = ui->treeWidget_laser->findItems("电流", Qt::MatchExactly);
            //            itemList.first()->setText(1, info.current);

            itemList = ui->treeWidget_laser->findItems("温度", Qt::MatchExactly);
            //            itemList.first()->setText(1, info.temp);

            itemList = ui->treeWidget_laser->findItems("错误码", Qt::MatchExactly);
            //            itemList.first()->setText(1, info.error);
            break;
        default:
            break;
    }
}

void RadarWidget::showSampleData(const QVector<WaveExtract::WaveformInfo> &allCh, int status)
{
    //    ret = waveExtract->getWaveform(sysStatus.radarType, sampleData, allCh);

    if(status == -1)  // 耗时小于1ms
    {
        // ui->statusBar->showMessage("帧头标志数据错误", 3);
        return;
    }
    else if(status == -2)
    {
        // ui->statusBar->showMessage("实际数据长度小于理论数据长度", 3);
        return;
    }
    else if(status == -3)
    {
        // ui->statusBar->showMessage("通道标志数据错误", 3);
        return;
    }

    if(sysStatus.radarType == BspConfig::RADAR_TYPE_DOUBLE_WAVE)
    {
        if(doubleWaveConfig.data.size() < colorMap_X_max)
            doubleWaveConfig.data.append(allCh);
        else
        {
            doubleWaveConfig.data.removeFirst();
            doubleWaveConfig.data.append(allCh);
        }
        doubleWaveConfig.sampleCnt++;
    }
    displayColorMap(allCh);

    if(ui->checkBox_isRerfreshUI->isChecked())
    {
        if(refreshUIFlag)
        {
            refreshUIFlag = false;

            // 刷新实时数据曲线
            if(sysStatus.radarType == BspConfig::RADAR_TYPE_LAND)
            {
                for(int n = 0; n < 6; n++)
                    ui->sampleDataPlot->graph(n)->data().data()->clear();

                for(int n = 0; n < allCh.size(); n++)
                    ui->sampleDataPlot->graph(n)->setData(allCh[n].pos, allCh[n].value);
            }
            else
            {
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
            }
            if(autoZoomPlot)
                ui->sampleDataPlot->rescaleAxes();
            ui->sampleDataPlot->replot();

            // 刷新伪彩色图
            if(allCh.size() == 8)
            {
                for(int m = 0; m < doubleWaveConfig.data.size(); m++)
                {
                    for(int i = 0; i < 4; i++)
                        updateColormap(i, m, allCh[i * 2 + 1].pos, allCh[i * 2 + 1].value);
                }
            }
            else
            {
                for(int m = 0; m < doubleWaveConfig.data.size(); m++)
                {
                    for(int i = 0; i < 4; i++)
                        updateColormap(i, m, allCh[i].pos, allCh[i].value);
                }
            }

            QCustomPlot *customPlot;
            for(int i = 0; i < 4; i++)
            {
                if(doubleWaveConfig.rescale[i])
                    widget2QCPColorMapList.at(i)->rescaleDataRange();
                customPlot = widget2CustomPlotList.at(i);
                customPlot->replot();
            }
        }
    }
}

void RadarWidget::displayColorMap(const QVector<WaveExtract::WaveformInfo> &allCh)
{
    if(sysStatus.radarType == BspConfig::RADAR_TYPE_WATER_GUARD)
    {
        //        QVector<WaveExtract::WaveformInfo> debugCh;
        //        debugCh.append(allCh[0]);
        //        debugCh.append(allCh[2]);
        //        debugCh.append(allCh[4]);
        //        debugCh.append(allCh[6]);

        //        allCh.clear();
        //        allCh.append(debugCh[0]);
        //        allCh.append(debugCh[1]);
        //        allCh.append(debugCh[2]);
        //        allCh.append(debugCh[3]);

        if(allCh.size() == 4)
        {
            int     valid_angle_cnt = 0;
            quint32 angle_90_offset = 109778;
            quint32 start_range     = angle_90_offset - 40960;
            quint32 stop_range      = angle_90_offset + 40960;
            double  angle;
            angle = (allCh[0].motorCnt - start_range) * 360 / 163840.0;  // 角度偏移修正

            // 通道0的波峰位置作为起点，截取后面的数据作为有效数据，这样波形相减时结果还好点
            int max_point_pos = Common::maxIndexInVector(allCh[0].value);

            QVector<WaveExtract::WaveformInfo> reduce;
            reduce.resize(4);
            for(int i = 0; i < 4; i++)
                reduce[i].value.resize(400);
#if 1
            switch(waterGuard.state)
            {
                case WaveExtract::MOTOR_CNT_STATE::IDLE:
                    if(waterGuard.startSaveBase)
                    {
                        waterGuard.base.clear();
                        waterGuard.base.resize(3);
                        waterGuard.state = WaveExtract::MOTOR_CNT_STATE::WAIT_START;
                        ui->btn_baseCapture->setEnabled(false);

                        ui->waterGuardBaseColor0->clearUI();
                        ui->waterGuardBaseColor1->clearUI();
                        ui->waterGuardBaseColor2->clearUI();
                    }
                    break;
                case WaveExtract::MOTOR_CNT_STATE::WAIT_START:
                    //采样率：2000Hz(0.5ms), 电机转速：120r/min, 163840/1000 = 163
                    // 电机计数值间隔大概163
                    if(allCh[0].motorCnt > start_range &&
                       allCh[0].motorCnt < start_range + 1000)
                    {
                        waterGuard.state = WaveExtract::MOTOR_CNT_STATE::LOAD_DATA;
                    }
                    break;

                case WaveExtract::MOTOR_CNT_STATE::LOAD_DATA:
                    // 0-180°的每次采样数据都保存起来
                    if(allCh[0].motorCnt < stop_range)
                    {
                        for(int i = 0; i < allCh.size(); ++i)
                        {  // 根据0通道峰值点，截取出作为基底的数据
                            reduce[i].value = allCh[i].value.mid(max_point_pos);
                        }

                        for(int i = 0; i < 3; i++)
                        {  // 保存每个通道的基本数据，用于比较
                            waterGuard.base[i].append(reduce[i + 1]);
                        }

                        ui->waterGuardBaseColor0->setData(reduce[1].value, angle);
                        ui->waterGuardBaseColor1->setData(reduce[2].value, angle);
                        ui->waterGuardBaseColor2->setData(reduce[3].value, angle);
                    }
                    else
                        waterGuard.state = WaveExtract::MOTOR_CNT_STATE::WAIT_END;
                    break;
                case WaveExtract::MOTOR_CNT_STATE::WAIT_END:

                    ui->waterGuardBaseColor0->refreshUI();
                    ui->waterGuardBaseColor1->refreshUI();
                    ui->waterGuardBaseColor2->refreshUI();

                    waterGuard.startSaveBase = false;
                    waterGuard.isSavedBase   = true;
                    waterGuard.state         = WaveExtract::MOTOR_CNT_STATE::IDLE;
                    ui->btn_baseCapture->setEnabled(true);

                    break;
                default:
                    waterGuard.startSaveBase = false;
                    break;
            }

#endif
            if(allCh[0].motorCnt < start_range && allCh[0].motorCnt > start_range - 1000)
            {
                ui->waterGuardTimeColor0->clearUI();
                ui->waterGuardTimeColor1->clearUI();
                ui->waterGuardTimeColor2->clearUI();
            }
            if(allCh[0].motorCnt > start_range && allCh[0].motorCnt < stop_range)
            {
                refreshRadarFlag = true;
#if 1
                if(waterGuard.isSavedBase == true)  // 有了基底后，要先减去基底
                {
                    for(int m = 0; m < 3; m++)  // 通道
                    {
                        const QVector<WaveExtract::WaveformInfo> &all_base_data = waterGuard.base[m];

                        if(valid_angle_cnt < all_base_data.size())  // 保证索引不会出界
                        {
                            const WaveExtract::WaveformInfo &angle_data = all_base_data[valid_angle_cnt];
                            QVector<double>                  data{400, 255};

                            const QVector<double> &temp = allCh[m + 1].value.mid(max_point_pos);

                            for(int n = 0, len1 = angle_data.value.size(), len2 = temp.size(); n < len1 && n < len2; n++)
                            {
                                data.append(temp[n] - angle_data.value[n]);
                            }
                            if(m == 0)
                                ui->waterGuardTimeColor0->setData(data, angle);
                            else if(m == 1)
                                ui->waterGuardTimeColor1->setData(data, angle);
                            else if(m == 2)
                                ui->waterGuardTimeColor2->setData(data, angle);
                        }
                    }
                }

                else
#endif
                {
                    ui->waterGuardTimeColor0->setData(allCh[1].value.mid(max_point_pos), angle);
                    ui->waterGuardTimeColor1->setData(allCh[2].value.mid(max_point_pos), angle);
                    ui->waterGuardTimeColor2->setData(allCh[3].value.mid(max_point_pos), angle);
                }
                valid_angle_cnt++;
            }
            else
            {
                valid_angle_cnt = 0;
#if 1
                if(refreshRadarFlag)
                {
                    refreshRadarFlag = false;
                    // 刷新雷达图
                    //                    if(sysStatus.radarType == BspConfig::RADAR_TYPE_WATER_GUARD)
                    {
                        ui->waterGuardTimeColor0->refreshUI();
                        ui->waterGuardTimeColor1->refreshUI();
                        ui->waterGuardTimeColor2->refreshUI();
                    }
                }
#endif
            }
        }
    }
}
