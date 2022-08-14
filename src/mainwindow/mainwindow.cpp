#include "mainwindow.h"
#include <algorithm>
#include "bsp_config.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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
