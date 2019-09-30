#include "mainwindow.h"
#include "radarchart.h"
#include "udpserver.h"

#include <QApplication>

#include <QMessageBox>
#include <QHostInfo>
#include <QDebug>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile qss("../Radar/qss/basic.qss");
//    QFile qss("basic.qss");
    qss.open(QFile::ReadOnly);
    a.setStyleSheet(qss.readAll());

    MainWindow w;
    w.show();

//    RadarChart c;
//    c.show();

    return a.exec();
}
