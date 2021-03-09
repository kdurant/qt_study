#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), configIni(new QSettings("../config.ini", QSettings::IniFormat)), thread(new QThread())
{
    ui->setupUi(this);

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

    epos2Driver = new EPOS2();

    devInfo           = new DevInfo();
    autoReadInfoTimer = new QTimer();
    autoReadInfoTimer->setInterval(1000);

    ssd = new SaveWave();

    offlineWaveForm->moveToThread(thread);
    connect(thread, SIGNAL(started()), offlineWaveForm, SLOT(getADsampleNumber()));
    connect(offlineWaveForm, SIGNAL(finishSampleFrameNumber()), thread, SLOT(quit()));

    //    connect(waveShow, SIGNAL(finishSampleFrameNumber()), waveShow, SLOT(deleteLater()));
    //    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    initParameter();
    uiConfig();

    udpBind();
    initSignalSlot();

    plotSettings();

    devInfo->getSysPara(sysParaInfo);
    ui->tableWidget_sysInfo->setColumnCount(2);
    ui->tableWidget_sysInfo->setRowCount(sysParaInfo.length());
    ui->tableWidget_sysInfo->setHorizontalHeaderLabels(QStringList() << "参数"
                                                                     << "值");
    ui->tableWidget_sysInfo->verticalHeader()->setVisible(false);  //隐藏列表头
    ui->tableWidget_sysInfo->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);  //x先自适应宽度
                                //    ui->tableWidget_sysInfo->horizontalHeader()
    //        ->setSectionResizeMode(0, QHeaderView::ResizeToContents); //然后设置要根据内容使用宽度的列

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

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initParameter()
{
    QString   localHostName = QHostInfo::localHostName();
    QHostInfo info          = QHostInfo::fromName(localHostName);

    localIP   = read_ip_address();
    localPort = configIni->value("System/localPort").toUInt();

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
        default:
            radarType = BspConfig::RADAR_TPYE_OCEAN;
            QMessageBox::critical(this, "error", "请设置正确的雷达类型");
            break;
    }

    deviceIP   = QHostAddress(configIni->value("System/radarIP").toString());
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

    sampleFrameNumber = 0;
}

void MainWindow::saveParameter()
{
    return;
}

void MainWindow::uiConfig()
{
    ui->rbtn_triggerInside->setChecked(true);
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
        ui->tabWidget->setTabEnabled(2, false);
        ui->tabWidget->setTabEnabled(5, false);
    }
    else if(radarType == BspConfig::RADAR_TPYE_OCEAN)
    {
        setWindowTitle(tr("海洋雷达控制软件"));
        ui->lineEdit_radarType->setText("海洋雷达");
        ui->label_laserCurrent->setVisible(false);
        ui->lineEdit_laserCurrent->setVisible(false);
        ui->comboBox_laserFreq->addItem("5000");
        //        ui->label
    }
    else if(radarType == BspConfig::RADAR_TPYE_LAND)
    {
        setWindowTitle(tr("陆地雷达控制软件"));
        ui->lineEdit_radarType->setText("陆地雷达");
        ui->label_triggerMode->setVisible(false);
        ui->rbtn_triggerInside->setVisible(false);
        ui->rbtn_triggerOutside->setVisible(false);
        ui->comboBox_laserFreq->addItem("100000");
        ui->comboBox_laserFreq->addItem("200000");
        ui->comboBox_laserFreq->addItem("400000");
    }
    else if(radarType == BspConfig::RADAR_TPYE_DRONE)
    {
        setWindowTitle(tr("无人机雷达控制软件"));
        ui->lineEdit_radarType->setText("无人机雷达");
        ui->label_laserPower->setVisible(false);
        ui->comboBox_laserPower->setVisible(false);
        ui->comboBox_laserFreq->addItem("4000");
        ui->label_compressLen->setVisible(false);
        ui->lineEdit_compressLen->setVisible(false);
        ui->label_compressRatio->setVisible(false);
        ui->lineEdit_compressRatio->setVisible(false);
    }
    else
    {
        setWindowTitle(tr("[xx]雷达控制软件"));
        ui->tabWidget->setTabEnabled(2, false);
        ui->tabWidget->setTabEnabled(5, false);
        ui->tabWidget->setTabEnabled(6, false);
        //        ui->tabWidget->setTabEnabled(7, false);
    }
    ui->checkBox_autoZoom->setChecked(true);

    QLabel *udpStatus = new QLabel("连接状态: ");
    ui->statusBar->addPermanentWidget(udpStatus);

    QLabel *captureStatus = new QLabel("采集状态：停止采集");
    captureStatus->setObjectName("captureStatus");
    captureStatus->setStyleSheet("color:red");
    ui->statusBar->addPermanentWidget(captureStatus);

    QLabel *labelVer = new QLabel();
    labelVer->setText("软件版本：v" + QString(SOFT_VERSION) + "_" + QString(GIT_DATE) + "_" + QString(GIT_HASH));
    ui->statusBar->addPermanentWidget(labelVer);
}

void MainWindow::udpBind()
{
    if(!localIP.contains("192.168.1"))
    {
        QMessageBox::warning(this, "警告", "请修改主机IP地址(192.168.1.xxx)");
        ui->statusBar->showMessage(tr("请修改主机IP地址(192.168.1.xxx"), 3);
        //        return;
    }

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

    /*
     * 读取系统参数信息相关逻辑
     */
    connect(devInfo, SIGNAL(sendDataReady(qint32, qint32, QByteArray &)), dispatch, SLOT(encode(qint32, qint32, QByteArray &)));
    connect(dispatch, SIGNAL(infoDataReady(QByteArray &)), devInfo, SLOT(setNewData(QByteArray &)));
    connect(ui->btn_ReadSysInfo, &QPushButton::pressed, this, &MainWindow::getSysInfo);
    connect(autoReadInfoTimer, &QTimer::timeout, this, &MainWindow::getSysInfo);
    connect(ui->checkBox_autoReadSysInfo, &QCheckBox::stateChanged, this, [this](int state) {
        if(state == Qt::Checked)
            autoReadInfoTimer->start();
        else
            autoReadInfoTimer->stop();
    });

    /*
     * 波形预览相关逻辑
     */
    connect(ui->btn_setPreviewPara, &QPushButton::pressed, this, [this]() {
        int totalSampleLen = ui->lineEdit_sampleLen->text().toInt();
        int previewRatio   = ui->lineEdit_sampleRate->text().toInt();
        int firstPos       = ui->lineEdit_firstStartPos->text().toInt();
        int firstLen       = ui->lineEdit_firstLen->text().toInt();
        int secondPos      = ui->lineEdit_secondStartPos->text().toInt();
        int secondLen      = ui->lineEdit_secondLen->text().toInt();

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
            status = 0;
            ui->btn_sampleEnable->setText("开始采集");
        }
        preview->setPreviewEnable(status);
    });

    connect(ui->checkBox_autoZoom, &QCheckBox::stateChanged, this, [this](int state) {
        if(state == Qt::Checked)
            autoZoomPlot = true;
        else
            autoZoomPlot = false;
    });

    connect(dispatch, SIGNAL(onlineDataReady(QByteArray &)), onlineWaveForm, SLOT(setNewData(QByteArray &)));
    connect(onlineWaveForm, &OnlineWaveform::fullSampleDataReady, this, [this](QByteArray &data) {
        QVector<quint8> sampleData;
        for(auto &i : data)  // 数据格式转换
            sampleData.append(i);
        QVector<WaveExtract::WaveformInfo> allCh;

        WaveExtract::getWaveform(radarType, sampleData, allCh);
        for(int n = 0; n < allCh.size(); n++)
        {
            if(allCh.size() != 8)  // 只有第一段波形
                ui->plot->graph(n * 2)->setData(allCh[n].pos, allCh[n].value);
            else
                ui->plot->graph(n)->setData(allCh[n].pos, allCh[n].value);
        }
        if(autoZoomPlot)
            ui->plot->rescaleAxes();
        ui->plot->replot();
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

    /*
     * Nor Flash操作，远程更新相关逻辑
     */

    connect(dispatch, SIGNAL(flashDataReady(QByteArray &)), updateFlash, SLOT(setDataFrame(QByteArray &)));
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
        connect(laser2Driver, SIGNAL(sendDataReady(qint32, qint32, QByteArray &)), dispatch, SLOT(encode(qint32, qint32, QByteArray &)));
        connect(dispatch, &ProtocolDispatch::laserDataReady, laser2Driver, &LaserType2::setNewData);
    }
    else if(radarType == BspConfig::RADAR_TPYE_DRONE)
    {
        connect(laser3Driver, SIGNAL(sendDataReady(qint32, qint32, QByteArray &)), dispatch, SLOT(encode(qint32, qint32, QByteArray &)));
        connect(dispatch, &ProtocolDispatch::laserDataReady, laser3Driver, &LaserType3::setNewData);
    }

    connect(ui->btn_laserOpen, &QPushButton::pressed, this, [this]() {
        bool status = false;
        switch(radarType)
        {
            case BspConfig::RADAR_TPYE_LAND:
                laser2Driver->setFreq(4000);
                status = laser2Driver->open();
                break;
            case BspConfig::RADAR_TPYE_DRONE:
                laser3Driver->setFreq(4000);
                status = laser3Driver->open();
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
            default:
                break;
        }
        if(!status)
            QMessageBox::warning(this, "警告", "指令流程异常，请尝试重新发送");
    });

    connect(ui->btn_laserSetCurrent, &QPushButton::pressed, this, [this]() {
        if(!laser2Driver->setCurrent(ui->lineEdit_laserCurrent->text().toInt()))
            QMessageBox::warning(this, "警告", "指令流程异常，请尝试重新发送");
    });

    connect(ui->btn_laserReadCurrent, &QPushButton::pressed, this, [this]() {
        QString text = laser2Driver->getCurrent();
        ui->lineEdit_laserShowCurrent->setText(text);
        laser3Driver->getStatus();
    });

    connect(ui->btn_laserReadFreq, &QPushButton::pressed, this, [this]() {
        QString text = laser2Driver->getFreq();
        ui->lineEdit_laserShowFreq->setText(text);
    });

    connect(ui->btn_laserReadTem, &QPushButton::pressed, this, [this]() {
        QString text = laser2Driver->getTemp();
        ui->lineEdit_laserShowTemp->setText(text);
    });

    connect(ui->rbtn_triggerInside, &QRadioButton::clicked, this, [this]() {
        laser3Driver->setMode(LaserController::IN_SIDE);
    });

    connect(ui->rbtn_triggerOutside, &QRadioButton::clicked, this, [this]() {
        laser3Driver->setMode(LaserController::OUT_SIDE);
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
        ui->btn_motorInit->setEnabled(true);
    });

    connect(ui->btn_motorMoveHome, &QPushButton::pressed, this, [this]() {
        ui->btn_motorMoveHome->setEnabled(false);
        epos2Driver->moveToHome();
        ui->btn_motorMoveHome->setEnabled(true);
    });

    connect(ui->btn_motorMovePostion, &QPushButton::pressed, this, [this]() {
        quint32 position = (ui->lineEdit_motorTargetPosition->text().toDouble(nullptr) / 360) * 163840;
        epos2Driver->moveToPosition(position);
    });

    /*
     * 采集数据保存相关逻辑
     */
    connect(ssd, SIGNAL(sendDataReady(qint32, qint32, QByteArray &)), dispatch, SLOT(encode(qint32, qint32, QByteArray &)));
    connect(dispatch, &ProtocolDispatch::ssdDataReady, ssd, &SaveWave::setNewData);
    connect(ui->btn_ssdSearchSpace, &QPushButton::pressed, this, [this]() {
        ui->btn_ssdSearchSpace->setEnabled(false);
        SaveWave::ValidFileInfo fileInfo;
        bool                    status    = false;
        quint32                 startUnit = ui->lineEdit_ssdSearchStartUnit->text().toUInt();

        status = ssd->inquireSpace(startUnit, fileInfo);
        if(status == false)
        {
            ui->lineEdit_ssdAvailFileUnit->setText(QString::number(fileInfo.fileUnit + 2, 16));
            ui->lineEdit_ssdAvailDataUnit->setText(QString::number(fileInfo.endUnit + 1, 16));
        }
        ui->btn_ssdSearchSpace->setEnabled(true);
    });

    connect(ui->btn_ssdEnableStore, &QPushButton::pressed, this, [this]() {
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

    connect(ui->btn_ssdEnableStore, &QPushButton::pressed, this, [this]() {
        QMessageBox message(QMessageBox::NoIcon, "停止存储", "真的要停止存储吗", QMessageBox::Yes | QMessageBox::No, NULL);
        if(message.exec() == QMessageBox::No)
            return;
        ui->btn_ssdEnableStore->setEnabled(true);
        ssd->enableStoreFile(0x00);
    });

    /*
     * DA设置相关逻辑
     */
    connect(ui->btn_DASetValue, &QPushButton::pressed, this, [this]() {
        quint32 chNum   = ui->comboBox_DAChSelect->currentIndex();
        double  DAValue = ui->lineEdit_DAValue->text().toDouble(nullptr);
        switch(radarType)
        {
            case BspConfig::RADAR_TPYE_LAND:
                DAValue = static_cast<quint32>((DAValue - 3.434) / 0.017);
                break;
            case BspConfig::RADAR_TPYE_OCEAN:
            case BspConfig::RADAR_TPYE_DRONE:
                if(chNum == 0)
                    DAValue = static_cast<quint32>((DAValue - 8.208) / 0.113);
                else if(chNum == 1)
                    DAValue = static_cast<quint32>((DAValue + 0.007) / 0.001);
                else if(chNum == 2)
                    DAValue = static_cast<quint32>((DAValue - 0.003) / 0.001);
                else if(chNum == 3)
                    DAValue = static_cast<quint32>((DAValue + 0.002) / 0.001);
                break;
            default:
                break;
        }
        daDriver->setChannalValue(chNum, DAValue);
        ui->plainTextEdit_DASetLog->appendPlainText(ui->comboBox_DAChSelect->currentText() + ": " + ui->lineEdit_DAValue->text() + "V");
    });

    /*
     * AD设置相关逻辑
     */
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
}

void MainWindow::plotSettings()
{
    //ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->plot->legend->setVisible(true);  //右上角指示曲线的缩略框
    ui->plot->xAxis->setLabel(QStringLiteral("时间：ns"));
    ui->plot->yAxis->setLabel(QStringLiteral("AD采样值"));

    for(int i = 0; i < 8; i++)
    {
        ui->plot->addGraph();
        ui->plot->graph(i)->setScatterStyle(QCPScatterStyle::ssDisc);
    }

    ui->plot->graph(0)->setPen(QPen(Qt::red));
    ui->plot->graph(0)->setName("通道0第一段");
    ui->plot->graph(1)->setPen(QPen(Qt::red));
    ui->plot->graph(1)->setName("通道0第二段");
    ui->plot->graph(2)->setPen(QPen(Qt::blue));
    ui->plot->graph(2)->setName("通道1第一段");
    ui->plot->graph(3)->setPen(QPen(Qt::blue));
    ui->plot->graph(3)->setName("通道1第二段");
    ui->plot->graph(4)->setPen(QPen(Qt::black));
    ui->plot->graph(4)->setName("通道2第一段");
    ui->plot->graph(5)->setPen(QPen(Qt::black));
    ui->plot->graph(5)->setName("通道2第二段");
    ui->plot->graph(6)->setPen(QPen(Qt::darkCyan));
    ui->plot->graph(6)->setName("通道3第一段");
    ui->plot->graph(7)->setPen(QPen(Qt::darkCyan));
    ui->plot->graph(7)->setName("通道3第二段");
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
            QVector<quint8> sampleData = offlineWaveForm->getFrameData(i);

            QVector<WaveExtract::WaveformInfo> allCh;
            if(WaveExtract::getWaveform(radarType, sampleData, allCh) == -1)
            {
                QMessageBox::warning(this, "警告", "数据格式和当前雷达类型不匹配");
                return;
            }
            for(int n = 0; n < allCh.size(); n++)
            {
                ui->plot->graph(n)->setData(allCh[n].pos, allCh[n].value);
            }
            if(autoZoomPlot)
                ui->plot->rescaleAxes();
            ui->plot->replot();

            if(interval_time == 0)
                continue;
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
    QVector<DevInfo::ParaInfo> info;
    if(devInfo->getSysPara(info))
    {
        for(int i = 0; i < sysParaInfo.length(); i++)
        {
            if(i == 0)
                ui->tableWidget_sysInfo->setCellWidget(i, 1, new QLabel(sysParaInfo[i].value));
            else
                ui->tableWidget_sysInfo->setCellWidget(i, 1, new QLabel(QString::number(sysParaInfo[i].value.toHex().toUInt(nullptr, 16))));
        }
    }
    else
    {
        QMessageBox::warning(this, "警告", "系统未连接");
    }
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
