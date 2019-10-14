#include "mainwindow.h"
#include "ui_mainwindow.h"

QQueue<QByteArray> adOrigData;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->hide();
    chart = new RadarChart();
    ui->horizontalLayout->addWidget(chart);

    initParameter();

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
    ui->lineEdit_localIP->setText(info.addresses()[1].toString());

    QSettings *configIni = new QSettings(":/config/config.ini", QSettings::IniFormat);
    //configIni->setValue("System/RadarType", "land");
    ui->lineEdit_laser_freq->setText(configIni->value("Laser/freq").toString());

    ui->lineEdit_localPort->setText(configIni->value("System/localPort").toString());
    ui->lineEdit_oceanIP->setText(configIni->value("System/oceanIP").toString());
    ui->lineEdit_oceanPort->setText(configIni->value("System/oceanPort").toString());

    ui->lineEdit_landIP->setText(configIni->value("System/landIP").toString());
    ui->lineEdit_landPort->setText(configIni->value("System/landPort").toString());

    ui->lineEdit_sampleLen->setText(configIni->value("Preview/sampleLen").toString());
    ui->lineEdit_sampleRate->setText(configIni->value("Preview/sampleRate").toString());
    ui->lineEdit_firstStartPos->setText(configIni->value("Preview/firstStartPos").toString());
    ui->lineEdit_firstLen->setText(configIni->value("Preview/firstLen").toString());
    ui->lineEdit_secondStartPos->setText(configIni->value("Preview/secondStartPos").toString());
    ui->lineEdit_secondLen->setText(configIni->value("Preview/secondLen").toString());
    ui->lineEdit_sumThreshold->setText(configIni->value("Preview/sumThreshold").toString());
    ui->lineEdit_subThreshold->setText(configIni->value("Preview/subThreshold").toString());

    if(configIni->value("System/radarType").toString() == "ocean")
        ui->rBtn_ocean->setChecked(true);
    else if(configIni->value("System/radarType").toString() == "land")
        ui->rBtn_land->setChecked(true);

    int         data;
    QScrollBar *verBar = ui->scrollArea->verticalScrollBar();
    data               = verBar->minimum();
    data               = verBar->maximum();
}

void MainWindow::saveParameter()
{
    QSettings *configIni = new QSettings(":/config/config.ini", QSettings::IniFormat);
       configIni->setValue("System/RadarType", "land");
//    configIni->setValue("Laser/freq", ui->lineEdit_laser_freq->text().toInt());
    configIni->setValue("Laser/freq", 1111);
    qDebug() << "save parameter";
}

void MainWindow::udpBind()
{
    udpSocket = new QUdpSocket(this);
    if(!udpSocket->bind(QHostAddress(ui->lineEdit_localIP->text()), ui->lineEdit_localPort->text().toInt()))
        QMessageBox::warning(NULL, "警告", "雷达连接失败");
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
        // 需要先判断数据内容，不是AD数据直接解析
        adOrigData.push_back(datagram);
        qDebug() << adOrigData.size();
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
    QByteArray   frame;
    QHostAddress addr;
    quint16      port;

    if(ui->rBtn_ocean->isChecked())
    {
        addr = QHostAddress(ui->lineEdit_oceanIP->text());
        port = ui->lineEdit_oceanPort->text().toInt();
    }
    else if(ui->rBtn_land->isChecked())
    {
        addr = QHostAddress(ui->lineEdit_landIP->text());
        port = ui->lineEdit_landPort->text().toInt();
    }

    frame = p.encode(SAMPLELEN, 4, ui->lineEdit_sampleLen->text().toInt());
    udpSocket->writeDatagram(frame.data(), frame.size(), addr, port);

    frame = p.encode(SAMPLERATE, 4, ui->lineEdit_sampleRate->text().toInt());
    udpSocket->writeDatagram(frame.data(), frame.size(), addr, port);

    frame = p.encode(FIRSTSTARTPOS, 4, ui->lineEdit_firstStartPos->text().toInt());
    udpSocket->writeDatagram(frame.data(), frame.size(), addr, port);

    frame = p.encode(FIRSTLEN, 4, ui->lineEdit_firstLen->text().toInt());
    udpSocket->writeDatagram(frame.data(), frame.size(), addr, port);

    frame = p.encode(SECONDLEN, 4, ui->lineEdit_secondLen->text().toInt());
    udpSocket->writeDatagram(frame.data(), frame.size(), addr, port);

    frame = p.encode(SECONDSTARTPOS, 4, ui->lineEdit_secondStartPos->text().toInt());
    udpSocket->writeDatagram(frame.data(), frame.size(), addr, port);

    frame = p.encode(SUMTHRESHOLD, 4, ui->lineEdit_sumThreshold->text().toInt());
    udpSocket->writeDatagram(frame.data(), frame.size(), addr, port);

    frame = p.encode(SUBTHRESHOLD, 4, ui->lineEdit_subThreshold->text().toInt());
    udpSocket->writeDatagram(frame.data(), frame.size(), addr, port);
}

void MainWindow::on_pushButton_sampleEnable_clicked()
{
    QByteArray   frame;
    QHostAddress addr;
    quint16      port;
    quint32      status;
    if(ui->rBtn_ocean->isChecked())
    {
        addr = QHostAddress(ui->lineEdit_oceanIP->text());
        port = ui->lineEdit_oceanPort->text().toInt();
    }
    else if(ui->rBtn_land->isChecked())
    {
        addr = QHostAddress(ui->lineEdit_landIP->text());
        port = ui->lineEdit_landPort->text().toInt();
    }

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
    frame = p.encode(SAMPLEENABLE, 4, status);
    udpSocket->writeDatagram(frame.data(), frame.size(), addr, port);
}
