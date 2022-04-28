#include "navigation.h"
#include "ui_navigation.h"
#include <cmath>

Navigation::Navigation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Navigation)
{
    ui->setupUi(this);

    initUI();
    initSignalSlot();

    //    getTestData();

    // qDebug() << "distance = " << distanceBetweenPoints(110.429545, 18.677179, 110.406169, 18.677106);
    // qDebug() << "distance = " << distanceBetweenPoints(110.415955, 18.686890, 110.416025, 18.666448);
    // qDebug() << "distance = " << distanceBetweenPoints(110.415955, 18.686890, 110.406169, 18.677106);
}

Navigation::~Navigation()
{
    delete ui;
}

void Navigation::initUI()
{
    ui->treeWidget_navigation->expandAll();
    ui->treeWidget_navigation->resizeColumnToContents(0);
    //    ui->treeWidget_navigation->setColumnWidth(1, 200);
    ui->groupBox_test->hide();
}

void Navigation::initSignalSlot()
{
    connect(ui->btn_loadTracker, &QPushButton::pressed, this, [this]() {
        QString updateFilePath = QFileDialog::getOpenFileName(this, tr(""), "", tr("*"));  //选择路径
        if(updateFilePath.size() == 0)
            return;
        ui->lineEdit_navigationFile->setText(updateFilePath);

        getTestData();
    });

    connect(ui->btn_loadMap, &QPushButton::pressed, this, [&] {
        mapPath            = QFileDialog::getExistingDirectory();
        QFileInfoList list = Common::getFileList(mapPath);
        if(list.length() != 16)
        {
            QMessageBox::critical(this, "error", "请选择正确的瓦片地图层级");
        }
        qDebug() << list;
    });
}

void Navigation::showGpsInfo(const BspConfig::Gps_Info &gps)
{
    QList<QTreeWidgetItem *> itemList;

    itemList = ui->treeWidget_navigation->findItems("GPS信息", Qt::MatchExactly);
    itemList.first()->child(0)->setText(1, QString::number(gps.week));
    itemList.first()->child(1)->setText(1, QString::number(gps.current_week_ms, 'g', 10));
    itemList.first()->child(2)->setText(1, QString::number(gps.sub_time / 10000000.0, 'g', 6));
    itemList.first()->child(3)->setText(1, QString::number(gps.latitude, 'g', 6));
    itemList.first()->child(4)->setText(1, QString::number(gps.longitude, 'g', 6));
    itemList.first()->child(5)->setText(1, QString::number(gps.altitude, 'g', 6));
    itemList.first()->child(6)->setText(1, QString::number(gps.roll, 'g', 6));
    itemList.first()->child(7)->setText(1, QString::number(gps.pitch, 'g', 6));
    itemList.first()->child(8)->setText(1, QString::number(gps.heading, 'g', 6));
}

void Navigation::showSystemInfo(double speed)
{
    QList<QTreeWidgetItem *> itemList;

    itemList = ui->treeWidget_navigation->findItems("系统参数", Qt::MatchExactly);
    itemList.first()->child(0)->setText(1, QString::number(speed));
}

int Navigation::getTestData()
{
#if 0
    QString filepath = "2017_09_21_09_49_55_772.datagps";

    QFile file(filepath);
    file.open(QIODevice::ReadOnly);
    QByteArray line;
    QByteArray temp{4, char(0)};

    int                 offset = 3;
    BspConfig::Gps_Info gps{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    while(!file.atEnd())
    {
        line = file.read(61);

        temp[0]             = line[offset + 2];
        temp[1]             = line[offset + 3];
        temp[2]             = line[offset + 4];
        temp[3]             = line[offset + 5];
        gps.current_week_ms = Common::ba2int(temp, 0) / 1000.0;

        temp[0]       = line[offset + 22];
        temp[1]       = line[offset + 23];
        temp[2]       = line[offset + 24];
        temp[3]       = line[offset + 25];
        gps.longitude = Common::ba2int(temp, 0) / 10000000.0;

        temp[0]      = line[offset + 18];
        temp[1]      = line[offset + 19];
        temp[2]      = line[offset + 20];
        temp[3]      = line[offset + 21];
        gps.latitude = Common::ba2int(temp, 0) / 10000000.0;

        temp[0]    = line[offset + 26];
        temp[1]    = line[offset + 27];
        temp[2]    = line[offset + 28];
        temp[3]    = line[offset + 29];
        gps.height = Common::ba2int(temp, 0) / 1000.0;

        Common::sleepWithoutBlock(20);
        setPostion(gps);
        qDebug() << "gps.longitude = " << gps.longitude
                 << "gps.latitude = " << gps.latitude;
    }
#else
    // 0.00007300000000043383x + (-0.023375999999998953)y = -0.42853637951893253
    // y = (0.00007300000000043383x + 0.42853637951893253) / (-0.023375999999998953)

    BspConfig::Gps_Info gps{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    gps.height = 400;
    double x   = 110.429545;
    while(x > 110.406169)
    {
        gps.longitude = x;
        gps.latitude  = (0.00007300000000043383 * x + 0.42853637951893253) / (0.023375999999998953);
        x -= 0.0001;

        Common::sleepWithoutBlock(50);

        qDebug() << "gps.longitude = " << gps.longitude
                 << "gps.latitude = " << gps.latitude;
    }
#endif
    return 0;
}
