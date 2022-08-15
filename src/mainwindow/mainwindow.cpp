#include "mainwindow.h"
#include <algorithm>
#include "bsp_config.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    configIni(new QSettings("./config.ini", QSettings::IniFormat))
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);
    setWindowTitle("雷达控制软件v" + QString(SOFT_VERSION) + "_" + GIT_DATE + "_" + GIT_HASH);

    generateDefaultConfig();

    initParameter();

    radarNumber = configIni->value("System/number").toInt();

    localIP = read_ip_address();

    if(localIP.length() == 0)
    {
        QMessageBox::warning(this, "警告", "没有合适的IP地址或网络连接异常");
    }
    else if(localIP.length() == 2)
    {
        QMessageBox::information(this, "通知", "检测本机有多个网卡在192.168.1.xxx网段，如果是连接多个雷达设备，请将连接陆地雷达的网卡IP设置为192.168.1.155，且先打开陆地雷达控制软件");
        radar1Para.localIP = localIP.at(0);
        radar2Para.localIP = localIP.at(1);
    }

    if(radarNumber == 1)
    {
        if(configIni->contains("RadarType1/radarType"))
        {
            radar1Para.radarType = BspConfig::RadarType(configIni->value("RadarType1/radarType").toInt());
            configRadar(radar1Para);

            radar1 = new RadarWidget(radar1Para, this);
            ui->tabWidget_main->addTab(radar1, radar1Para.name);
        }
        else
            QMessageBox::warning(this, "警告", "配置文件出错");
    }
    else if(radarNumber == 2)
    {
        radar1Para.radarType = BspConfig::RadarType(configIni->value("RadarType1/radarType").toInt());
        configRadar(radar1Para);
        radar1 = new RadarWidget(radar1Para, this);
        ui->tabWidget_main->addTab(radar1, radar1Para.name);

        radar2Para.radarType = BspConfig::RadarType(configIni->value("RadarType2/radarType").toInt());
        configRadar(radar2Para);
    }
    else
        QMessageBox::warning(this, "警告", "配置文件出错");

    if(isHaveLand)
    {
        radar1 = new RadarWidget(radar1Para, this);
        ui->tabWidget_main->addTab(radar1, "陆地雷达");
    }
    if(ishaveOcean)
    {
        radar2 = new RadarWidget(radar2Para, this);
        ui->tabWidget_main->addTab(radar2, "海洋雷达");
    }
    setToolBar();

    note         = new NoteInfo;
    engineerView = new Navigation;
    pilotView    = new Navigation;

    timer1s = startTimer(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initParameter()
{
    if(configIni->value("System/mode").toString() == "debug")
    {
        // localIP.append("127.0.0.1");
        QMessageBox::information(this, "通知", "当前为调试模式, IP addr:127.0.0.1");
    }
}

void MainWindow::generateDefaultConfig()
{
    QFileInfo fileInfo("./config.ini");
    if(!fileInfo.exists())
    {
        QFile file("./config.ini");
        file.open(QIODevice::WriteOnly);
        file.write("; RADAR_TPYE_OCEAN = 0x00,\r\n");
        file.write("; RADAR_TPYE_LAND = 0x01,\r\n");
        file.write("; RADAR_TPYE_760 = 0x02,\r\n");
        file.write("; RADAR_TPYE_DOUBLE_WAVE = 0x03,\r\n");
        file.write("; RADAR_TPYE_DRONE = 0x04,\r\n");
        file.write("; RADAR_TPYE_WATER_GUARD = 0x05,\r\n");
        file.write("; RADAR_TPYE_SECOND_INSTITUDE = 0x06,\r\n");
        file.write("; RADAR_TPYE_BIG_FLARE = 0x07,\r\n");

        file.write("\r\n[System]\r\n");
        file.write("; release or debug\r\n");
        file.write("mode=debug\r\n");
        file.write("number=2\r\n");

        file.write("\r\n[RadarType1]\r\n");
        file.write("radarType=1\r\n");
        // file.write("radarType1=1\r\n");
        // file.write("radarType2=1\r\n");
        // file.write("compressLen=0\r\n");
        // file.write("compressRatio=0\r\n");
        file.write("sampleLen=6000\r\n");
        file.write("sampleRate=20000\r\n");
        file.write("firstStartPos=32\r\n");
        file.write("firstLen=64\r\n");
        file.write("secondStartPos=1280\r\n");
        file.write("secondLen=200\r\n");
        file.write("subThreshold=200\r\n");
        file.write("sumThreshold=2000\r\n");

        file.write("\r\n[RadarType2]\r\n");
        file.write("radarType=0\r\n");
        file.write("sampleLen=6000\r\n");
        file.write("sampleRate=1000\r\n");
        file.write("firstStartPos=32\r\n");
        file.write("firstLen=100\r\n");
        file.write("secondStartPos=1000\r\n");
        file.write("secondLen=400\r\n");
        file.write("subThreshold=200\r\n");
        file.write("sumThreshold=2000\r\n");

        file.close();
    }
}

QStringList MainWindow::read_ip_address()
{
    QStringList         ips;
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach(QHostAddress address, list)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            QString ip = address.toString();
            if(ip.contains("192.168.1."))
                ips.append(ip);
        }
    }
    std::sort(ips.begin(), ips.end());
    return ips;
}

void MainWindow::setToolBar()
{
    QVector<QAction *> act;
    act.append(new QAction("帮助", this));
    act.append(new QAction("导航", this));
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    for(int i = 0; i < act.size(); i++)
    {
        ui->mainToolBar->addAction(act[i]);
    }

    connect(act[0], &QAction::triggered, this, [this]()
            {
        note->show();
    });
    connect(act[1], &QAction::triggered, this, [this]()
            {
        engineerView->show();
        // pilotView->show();
        //        if(radarType == BspConfig::RADAR_TYPE_LAND)
        //        {
        //            engineerView->setScanAngle(60);
        //            // pilotView->setScanAngle(60);
        //        }
        //        else
        //        {
        //            engineerView->setScanAngle(30);
        //            // pilotView->setScanAngle(30);
        //        }
    });
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(timer1s == event->timerId())
    {
        QString udpStatus;

        ui->statusBar->showMessage(udpStatus, 3000);
    }
}
