#include "mainwindow.h"
#include "ui_mainwindow.h"

QQueue<QString> adOrigData;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("雷达控制软件"));

    configIni = new QSettings("./config.ini", QSettings::IniFormat);

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

    ui->rBtn_radarType->setChecked(true);
    ui->rBtn_radarType->setText(radarType + "雷达");
}

void MainWindow::saveParameter()
{
    configIni->setValue("System/RadarType", "land");
    //    configIni->setValue("Laser/freq", ui->lineEdit_laser_freq->text().toInt());
    configIni->setValue("Laser/freq", 1111);
    qDebug() << "--------------------------------run end----------------------------------------------";
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
    ui->gb_laser->hide();
    ui->gb_motor->hide();
    ui->gb_volt->hide();
    //    ui->gb_preview->hide();
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
}

void MainWindow::processPendingDatagram()
{
    QByteArray datagram;
    QString    data;
    int        len;
    while(udpSocket->hasPendingDatagrams())
    {
        len = udpSocket->pendingDatagramSize();
        datagram.resize(len);
        udpSocket->readDatagram(datagram.data(), datagram.size());
        protocol->setDataFrame(datagram);
        //        protocol

        data = datagram.toHex();
        // 需要先判断数据内容，不是AD数据直接解析
        //        if(data.mid(COMMAND_POS, COMMAND_LEN) == "80000006")
        //            adOrigData.push_back(data);

        //        if(adOrigData.size() > 10 && data.mid(PCK_NUMER_POS, PCK_NUMBER_LEN).toInt(nullptr, 16) == 0)
        //        {
        //            {
        //                protocol.get_single_ad_data(adOrigData);
        //                ui->graphicsView->updateChart(protocol.get_channal_data(0));
        //            }
        //        }
    }
}

void MainWindow::initSignalSlot()
{
    //    connect(this, &QUdpSocket::readyRead, udpSocket, udpSocket::
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagram()));
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
    //    QByteArray frame;
    //    qint32     data;

    //    if(radarType == RADAR_TYPE_760)
    //    {
    //        data  = ui->lineEdit_sampleRate->text().toInt();
    //        data  = (data / 8) * 8;
    //        frame = protocol.encode(SAMPLERATE, 4, data);
    //        udpSocket->writeDatagram(frame.data(), frame.size(), deviceIP, devicePort);

    //        data  = ui->lineEdit_firstStartPos->text().toInt();
    //        data  = (data / 8) * 8;
    //        frame = protocol.encode(FIRSTSTARTPOS, 4, data);
    //        udpSocket->writeDatagram(frame.data(), frame.size(), deviceIP, devicePort);

    //        data  = ui->lineEdit_firstLen->text().toInt();
    //        data  = (data / 8) * 8;
    //        frame = protocol.encode(FIRSTLEN, 4, data);
    //        udpSocket->writeDatagram(frame.data(), frame.size(), deviceIP, devicePort);
    //    }
    /*
    frame = protocol.encode(SAMPLELEN, 4, ui->lineEdit_sampleLen->text().toInt());
    udpSocket->writeDatagram(frame.data(), frame.size(), deviceIP, devicePort);

    frame = protocol.encode(SECONDLEN, 4, ui->lineEdit_secondLen->text().toInt());
    udpSocket->writeDatagram(frame.data(), frame.size(), deviceIP, devicePort);

    frame = protocol.encode(SECONDSTARTPOS, 4, ui->lineEdit_secondStartPos->text().toInt());
    udpSocket->writeDatagram(frame.data(), frame.size(), deviceIP, devicePort);

    frame = protocol.encode(SUMTHRESHOLD, 4, ui->lineEdit_sumThreshold->text().toInt());
    udpSocket->writeDatagram(frame.data(), frame.size(), deviceIP, devicePort);

    frame = protocol.encode(SUBTHRESHOLD, 4, ui->lineEdit_subThreshold->text().toInt());
    udpSocket->writeDatagram(frame.data(), frame.size(), deviceIP, devicePort);
    */
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
    //    frame = protocol.encode(SAMPLEENABLE, 4, status);
    //    udpSocket->writeDatagram(frame.data(), frame.size(), deviceIP, devicePort);
}

void MainWindow::on_pushButton_laserInfo_clicked()
{
    //    QString frame;
    //    frame = "aa555aa5aa555aa50000043b8000000600000000000001000123456789abcdef0000000000000000000000005b659b7600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011000000000000000400001770eb90a55a00000020014000ef00e700e100de00e000e400ea00f50103011401240135014c0162017a019401ab01c101d901f102060219022d023902470250025a026002630260025e0253024b023f022f021d020801f401df01c701af0195017d01670150013c01290117010800fb00ef00e800e200df00e000e400eb00f30101011201220138014a0162017a019201aa01c301d901f002040218022a023c024a02520259025f0262913eb0f3";
    //    adOrigData.enqueue(frame);
    //    frame = "aa555aa5aa555aa50000043c8000000600000001000001000260025d0256024a023d022f021d020901f501de01c701ad0196017d01660150013c012b0117010800f800ee00e600e100dd00df00e400ea00f50102011001230137014b01620179019101aa01c201da01f102050218022a0239024602500259025f02610261025d0255024a023f0230021c020901f401df01c601ac0197017f01680152013c01290117010800fa00ee00e800e200e000e100e500eb00f50101011201230135014a0162017b019001a901c101d801ef02050218022a023a02470250025a025c02600261025f0254024c023e0230021f020b01f601dc01c601af0196017e01670152013b01290117010600fa00f000e700e000de00e100e400eae6fc6b8f";
    //    adOrigData.enqueue(frame);
    //    frame = "aa555aa5aa555aa50000043d80000006000000020000010000f70102011201220136014c0162017a019201ab01c201d801f002050219022c023a02470250025a025f02620261025e0255024b023e022f021e020801f501df01c601ae0196017e01670151013c01280116010700fa00ed00e700e200e100e000e300eb00f50102011301240136014901610179019201a901c301d801f002030218022a023b02470251025a025f02620262025c0254024c023e022f021f020d01f301de01c701ae0196017e01670151013c012a0118010700fa00ef00e600e200df00e100e500ea00f60102011301230136014b0161017a019101ab01c101d901efeb90a55a0f0f0020014000d200d000d200d200d100d000cf00d100d000d084c15ef0";
    //    adOrigData.enqueue(frame);
    //    frame = "aa555aa5aa555aa50000043e80000006000000030000010000d100d100d200d100d000cf00d200d100cf00d000cf00cf00d000d100d100d000d000d100d000d100d000d100d100d000d000d000d000d000d000d000d000d100d100d000d100d200d100cf00d000d100d200d000d000d200d000d100d000d000d100d000d000d100d000d000d000d000cf00d000d000d000d200d200d100d000d000d000d000d100d000d000d100d200d100d000d100d100d100d100d100d000d100d200d100d100d200d100d000d100d200d000d000d100d000d000d000d100d000d100d000d000d100d200cf00d200cf00d000cf00d200d100cf00d100cf00d100d100d200d000d100d000d000d200d200d100d000d100d100d100d000d0b41f396c";
    //    adOrigData.enqueue(frame);
    //    frame = "aa555aa5aa555aa50000043f80000006000000040000010000d100d000d100d100d100d100d100d000d000cf00d000d000d100d100d200cf00d000cf00cf00d000cf00d100d100d000d000cf00cf00cf00d100cf00d000cf00d000cf00d000d100d000d100cf00d100d100d000d000d100d000cf00d100d000d100d000d000d200d200d200d100cf00d000d000d100d000d000cf00d000d100d000d000d000d000cf00d100d100d100d100d100d100d100d100d000d000d100d100d100d200d100d100cf00d100d100d100ce00d000cf00d000d100d000d100d000d000d000d100d000d100d100d000d200d000d100d100d100d100cf00d000d000d100d000d000d000d000cf00d100d100d000d000ce00cf00d000d000d1b4183956";
    //    adOrigData.enqueue(frame);
    //    frame = "aa555aa5aa555aa50000044080000006000000050000010000d000d100d000d000d200d200ce00cf00d000d000d000d100d000cf00d100d000d200d000d200d000d100d000d100d100d200d100d100d000cf00d000d200d000d000cf00d000d000d100d100d000d000d000cf00d000d000cf00d100d200d100d100d100d000d000d000d0eb90a55af0f00020014000d700d600d700d700d600d800d600d600d700d600d700d600d700d800d900d600d600d700d600d500d600d700d700d700d600d600d800d700d500d500d600d700d700d600d500d500d700d600d500d600d600d500d600d600d700d600d600d700d700d600d600d500d700d700d600d700d700d600d700d500d700d700d700d800d600d700d600d700d79036de0f";
    //    adOrigData.enqueue(frame);
    //    frame = "aa555aa5aa555aa50000044180000006000000060000010000d700d500d600d600d800d700d700d700d700d700d700d700d700d600d600d700d700d600d700d800d600d600d800d700d600d700d600d800d700d700d700d600d800d700d700d700d600d700d700d700d800d500d600d700d600d600d600d600d600d600d700d600d600d700d700d600d700d800d600d900d600d700d700d600d700d800d700d600d600d700d600d700d500d700d700d700d800d600d600d800d800d600d800d700d600d600d700d800d800d700d700d600d600d600d600d700d700d600d600d600d800d600d800d700d700d700d700d700d700d500d600d700d800d700d700d600d600d700d800d800d700d700d600d600d700d700d700d7b5b33afa";
    //    adOrigData.enqueue(frame);
    //    frame = "aa555aa5aa555aa50000044280000006000000070000010000d600d700d700d600d700d700d600d600d600d600d700d800d600d600d700d600d700d600d600d700d600d600d600d600d600d600d600d700d800d700d700d500d600d700d600d700d600d700d500d600d600d600d700d700d800d800d600d600d500d800d600d700d700d700d700d700d800d800d800d500d700d700d800d700d700d700d600d600d500d700d700d700d600d600d700d600d600d600d600d900d700d600d700d600d700d700d500d700d500d600d700d500d800d600d700d800d700d700d700d700d700d700d700d600d600d500d700d700d700d600d500d600d800d600d700d800d700d700d500d500d700d700d8eb90a55affff002001405974253f";
    //    adOrigData.enqueue(frame);
    //    frame = "aa555aa5aa555aa50000044380000006000000080000010000d200d400d400d400d500d500d600d500d600d300d300d300d500d500d400d500d500d300d500d400d200d300d400d500d400d500d500d500d500d300d200d200d500d400d500d500d400d500d600d500d400d400d400d400d400d300d400d500d600d200d100d200d500d500d300d400d600d500d400d500d400d400d600d300d400d600d600d500d500d400d300d400d600d500d400d500d500d500d500d300d100d200d400d400d300d600d500d700d400d400d200d400d500d300d500d600d600d400d500d300d200d400d400d400d300d500d400d500d500d300d200d300d400d600d400d400d500d400d500d300d400d300d500d500d400d500d500d4b5103a5c";
    //    adOrigData.enqueue(frame);
    //    frame = "aa555aa5aa555aa50000044480000006000000090000010000d400d300d300d500d400d400d600d500d600d500d400d400d300d200d400d500d300d500d500d500d600d500d300d400d500d500d400d400d500d500d500d200d400d300d200d500d600d500d600d600d400d400d300d400d500d400d300d400d500d400d500d300d100d400d400d400d400d400d600d400d600d400d300d400d500d500d500d600d500d500d600d300d300d300d400d500d500d300d700d600d500d200d200d400d400d500d400d400d400d500d500d200d200d200d500d600d400d500d600d500d400d400d400d300d500d500d400d400d500d500d500d300d300d200d300d600d300d500d500d400d500d400d200d300d600d600d500d5b5153a60";
    //    adOrigData.enqueue(frame);
    //    frame = "aa555aa5aa555aa500000445800000060000000a0000008000d500d600d400d300d200d300d300d500d500d600d400d500d500d300d300d400d400d400d500d500d600d400d400d500d300d400d600d400d500d500d600d600d500d200d100d500d400d600d500d400d500d600d500d200d100d400d300d500d400d700d500d500d500d500d300d300d600d500d500d300d500d500d600d400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000009a8a1f64";
    //    adOrigData.enqueue(frame);

    //    frame = "aa555aa5aa555aa500000445800000060000000a0000008000d500d600d400d300d200d300d300d500d500d600d400d500d500d300d300d400d400d400d500d500d600d400d400d500d300d400d600d400d500d500d600d600d500d200d100d500d400d600d500d400d500d600d500d200d100d400d300d500d400d700d500d500d500d500d300d300d600d500d500d300d500d500d600d400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000009a8a1f64";
    //    adOrigData.enqueue(frame);

    //    protocol.get_single_ad_data(adOrigData);

    //    ui->graphicsView->updateChart(protocol.get_channal_data(0));
}
