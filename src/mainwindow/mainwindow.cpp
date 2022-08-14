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

    radarLand  = new RadarWidget(this);
    radarOcean = new RadarWidget(this, BspConfig::RADAR_TYPE_OCEAN);

    ui->tabWidget_main->addTab(radarLand, "陆地雷达");
    ui->tabWidget_main->addTab(radarOcean, "海洋雷达");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initParameter()
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
        file.write("radarType=1\r\n");
        file.write("\r\n[Preview]\r\n");
        file.write("compressLen=0\r\n");
        file.write("compressRatio=0\r\n");
        file.write("firstLen=32\r\n");
        file.write("firstStartPos=32\r\n");
        file.write("sampleLen=6000\r\n");
        file.write("sampleRate=4000\r\n");
        file.write("secondLen=128\r\n");
        file.write("secondStartPos=1280\r\n");
        file.write("subThreshold=200\r\n");
        file.write("sumThreshold=2000\r\n");

        file.close();
    }
}
