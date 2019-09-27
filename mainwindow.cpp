#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include "noteinfo.h"

#include <QCloseEvent>

#include <QSettings>
#include <QScrollBar>

#include <QHostInfo>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initParameter();
    saveParameter();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initParameter()
{

    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);
    ui->lineEdit_localIP->setText(info.addresses()[1].toString());


    QSettings *configIni = new QSettings("../Radar/config.ini", QSettings::IniFormat);
    //configIni->setValue("Laser/freq", 1000);
    //configIni->setValue("System/RadarType", "land");
    ui->lineEdit_laser_freq->setText(configIni->value("Laser/freq").toString());

    ui->lineEdit_localPort->setText(configIni->value("System/localPort").toString());
    ui->lineEdit_oceanIP->setText(configIni->value("System/oceanIP").toString());
    ui->lineEdit_oceanPort->setText(configIni->value("System/oceanPort").toString());

    ui->lineEdit_landIP->setText(configIni->value("System/landIP").toString());
    ui->lineEdit_landPort->setText(configIni->value("System/landPort").toString());

    if(configIni->value("System/radarType").toString() == "ocean")
        ui->rBtn_ocean->setChecked(true);
    else if(configIni->value("System/radarType").toString() == "land")
        ui->rBtn_land->setChecked(true);


    int data;
    QScrollBar *verBar = ui->scrollArea->verticalScrollBar();
    data = verBar->minimum();
    data = verBar->maximum();
}

void MainWindow::saveParameter()
{
    QSettings *configIni = new QSettings("../Radar/config.ini", QSettings::IniFormat);
    configIni->setValue("System/RadarType", "land");
    configIni->setValue("Laser/freq", 1000);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveParameter();
    qDebug() << "end";
}

void MainWindow::on_actionNote_triggered()
{
    std::cout << "nihao";
    NoteInfo *note = new NoteInfo;
    note->show();
}
