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


    QString t1 = "/home/wj/work/map1/14";
    ui->mapView->setMapPath(t1, 13186, 7342);
    ui->mapView->loadMap();
    QString t2 = "/home/wj/work/map1/tracker.txt";
    parseTrackerFile(t2, m_gps_routine);

    int len = m_gps_routine.length();
    for(int i = 0; i < len; i += 2)
    {
        ui->mapView->loadTracker(m_gps_routine[i], m_gps_routine[i + 1]);
        ui->mapView->loadSerialNum(m_gps_routine[i], i / 2 + 1);
    }
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
        parseTrackerFile(updateFilePath, m_gps_routine);

        int len = m_gps_routine.length();
        for(int i = 0; i < len; i += 2)
        {
            ui->mapView->loadTracker(m_gps_routine[i], m_gps_routine[i + 1]);
            ui->mapView->loadSerialNum(m_gps_routine[i], i / 2 + 1);
        }

        //        getTestData();
    });

    connect(ui->btn_loadMap, &QPushButton::pressed, this, [&] {
        mapPath            = QFileDialog::getExistingDirectory();
        QFileInfoList list = Common::getFileList(mapPath);
        if(list.length() != 16)
        {
            QMessageBox::critical(this, "error", "请选择正确的瓦片地图层级");
        }

        int len         = list[0].filePath().split('/').length();
        m_tile_X_offset = list[0].filePath().split('/')[len - 2].toUInt();
        m_tile_Y_offset = list[0].fileName().split('.')[0].toUInt();

        ui->mapView->setMapPath(mapPath, m_tile_X_offset, m_tile_Y_offset);
        ui->mapView->loadMap();

        qDebug() << list[0].filePath();
        qDebug() << list[0].fileName();
        qDebug() << m_tile_X_offset << "\n"
                 << m_tile_Y_offset;
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

void Navigation::parseTrackerFile(QString &path, QVector<QPointF> &track)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);

    auto gps_dfm2decmal = [](QByteArray &gps_dfm) -> double {
        QByteArray sep_d;
        sep_d.append(0xa1);
        sep_d.append(0xe3);
        QByteArray sep_m;
        sep_m.append(0xa1);
        sep_m.append(0xe4);
        QByteArray sep_s;
        sep_s.append(0xa1);
        sep_s.append(0xe5);

        int idx_d = gps_dfm.indexOf(sep_d);
        int idx_m = gps_dfm.indexOf(sep_m);
        int idx_s = gps_dfm.indexOf(sep_s);

        QString s_degree = gps_dfm.mid(0, idx_d);
        QString s_minute = gps_dfm.mid(idx_d + 2, idx_m - idx_d - 2);
        QString s_second = gps_dfm.mid(idx_m + 2, idx_s - idx_m - 2);

        double ret = s_degree.toUInt() + s_minute.toUInt() / 60.0 + s_second.toUInt() / 60.0 / 60.0;

        return ret;

        //        double du    = gps_dfm.indexOf('°')
    };
    while(!file.atEnd())
    {
        QByteArray        line = file.readLine();
        QList<QByteArray> list = line.split(',');

        double lng;
        double lat;
        lng = gps_dfm2decmal(line.split(',')[1]);
        lat = gps_dfm2decmal(line.split(',')[2]);
        track.append(QPointF(lng, lat));
        lng = gps_dfm2decmal(line.split(',')[3]);
        lat = gps_dfm2decmal(line.split(',')[4]);
        track.append(QPointF(lng, lat));
    }

    return;
}
