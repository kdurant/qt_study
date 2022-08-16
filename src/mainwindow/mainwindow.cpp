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
    radar.resize(radarNumber);

    if(radarNumber != localIP.length())
    {
        QMessageBox::warning(this, "警告", "雷达个数和系统IP地址个数不匹配，请检查系统配置");
    }

    for(m_loop_i = 0; m_loop_i < radarNumber; m_loop_i++)
    {
        item = "RadarType" + QString::number(m_loop_i + 1) + "/radarType";
        if(configIni->contains(item))
        {
            radar[m_loop_i].para.radarType = BspConfig::RadarType(configIni->value(item).toInt());
            configRadar(radar[m_loop_i].para);

            if(m_loop_i < localIP.length())
                radar[m_loop_i].para.localIP = localIP.at(m_loop_i);
            else
            {
                radar[m_loop_i].para.localIP = "127.0.0.1";
                QMessageBox::warning(this, "警告", "雷达个数和系统IP地址个数不匹配, 使用默认回环IP地址");
            }

            radar[m_loop_i].device = new RadarWidget(radar[m_loop_i].para, this);
            ui->tabWidget_main->addTab(radar[m_loop_i].device, radar[m_loop_i].para.name);
        }

        else
        {
            QMessageBox::warning(this, "警告", "配置文件出错, 系统即将退出");
            return;
        }
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
        localIP.append("127.0.0.1");
        localIP.append("127.0.0.1");
        QMessageBox::information(this, "通知", "当前为调试模式, IP addr:127.0.0.1");
    }
    else
        localIP = read_ip_address();
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
    QStringList ips;
    QString     ip;

    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    QList<QNetworkAddressEntry>    entrys;
    foreach(QNetworkInterface interface, interfaces)
    {
        entrys = interface.addressEntries();
        for(auto entery : entrys)
        {
            switch(entery.ip().protocol())
            {
                case QAbstractSocket::IPv4Protocol:
                    ip = entery.ip().toString();
                    if(ip.contains("192.168.1."))
                        ips.append(ip);
                    break;
                case QAbstractSocket::IPv6Protocol:
                    break;
                case QAbstractSocket::AnyIPProtocol:
                    break;
                case QAbstractSocket::UnknownNetworkLayerProtocol:
                    break;
            }
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

        QVector<_RadarVector_>::iterator iter;
        for(iter = radar.begin(); iter != radar.end(); iter++)
        {
            iter->para = iter->device->getRadarStatus();
            if(iter->para.udpLinkStatus)
                udpStatus.append(iter->para.name + "通信成功    ");
            else
                udpStatus.append(iter->para.name + "通信失败    ");
        }

        ui->statusBar->showMessage(udpStatus, 3000);
    }
}
