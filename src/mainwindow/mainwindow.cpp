#include "mainwindow.h"
#include <algorithm>
#include "bsp_config.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);

    QVBoxLayout *vbox1 = new QVBoxLayout;

    QWidget *radarLand = ui->tabWidget_main->widget(0);
    radarLand->setLayout(vbox1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
