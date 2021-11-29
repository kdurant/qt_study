#include "navigation.h"
#include "ui_navigation.h"
#include <cmath>

Navigation::Navigation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Navigation)
{
    ui->setupUi(this);

    initUI();
    plotSettings();
    initSignalSlot();

    //    getTestData();

    qDebug() << "distance = " << distanceBetweenPoints(110.429545, 18.677179, 110.406169, 18.677106);
    qDebug() << "distance = " << distanceBetweenPoints(110.415955, 18.686890, 110.416025, 18.666448);
    qDebug() << "distance = " << distanceBetweenPoints(110.415955, 18.686890, 110.406169, 18.677106);
}

Navigation::~Navigation()
{
    delete ui;
}

void Navigation::initUI()
{
    setWindowTitle("导航");
    ui->treeWidget_navigation->expandAll();
    ui->treeWidget_navigation->resizeColumnToContents(0);
    //    ui->treeWidget_navigation->setColumnWidth(1, 200);
    ui->navigationPlot->hide();
    ui->groupBox_test->hide();
    QPixmap pix("navigation.png");

    qDebug() << ui->label_info->size();
    QPixmap dest = pix.scaled(ui->label_info->size(), Qt::KeepAspectRatio);

    ui->label_info->setPixmap((pix));
    //    ui->label_info->lo
}

void Navigation::initSignalSlot()
{
    connect(ui->btn_loadTracker, &QPushButton::pressed, this, [this]() {
        QString updateFilePath = QFileDialog::getOpenFileName(this, tr(""), "", tr("*"));  //选择路径
        if(updateFilePath.size() == 0)
            return;
        ui->lineEdit_navigationFile->setText(updateFilePath);
        paserTracker(ui->lineEdit_navigationFile->text());

        while(ui->navigationPlot->plottableCount() > 0)
            ui->navigationPlot->removeGraph(0);

        qDebug() << "delete: plottableCount() = " << ui->navigationPlot->plottableCount();

        addTrackerGraph();
        for(int i = 0; i < targetGraph.size(); i++)
        {
            ui->navigationPlot->addGraph();
            ui->navigationPlot->graph(i + 1)->setScatterStyle(QCPScatterStyle::ssDisc);
            ui->navigationPlot->graph(i + 1)->setPen(QPen(Qt::blue));
            ui->navigationPlot->graph(i + 1)->setData(targetGraph[i].key, targetGraph[i].value);
        }

        ui->navigationPlot->rescaleAxes();
        ui->navigationPlot->replot();
        qDebug() << "add: plottableCount() = " << ui->navigationPlot->plottableCount();

        getTestData();
    });
}

void Navigation::plotSettings()
{
    //    ui->navigationPlot->legend->setVisible(true);  //右上角指示曲线的缩略框
    ui->navigationPlot->xAxis->setLabel(QStringLiteral("纬度"));
    ui->navigationPlot->yAxis->setLabel(QStringLiteral("经度"));

    ui->navigationPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                        QCP::iSelectLegend | QCP::iSelectPlottables);  // 可以按住鼠标拖动图标，鼠标滚轮缩放图表
}

void Navigation::addTrackerGraph()
{
    ui->navigationPlot->addGraph();

    ui->navigationPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssTriangle);
    ui->navigationPlot->graph(0)->setPen(QPen(Qt::red));
}

int Navigation::paserTracker(const QString &filepath)
{
    targetGraph.clear();
    historyTracker.key.clear();
    historyTracker.value.clear();

    QFile file(filepath);
    file.open(QIODevice::ReadOnly);

    while(!file.atEnd())
    {
        QByteArray  line = file.readLine();
        QString     str(line);
        QStringList strList = str.split(" ");
        if(strList.length() != 7)
            return -1;

        Line target;
        target.number          = strList[0].toInt(nullptr, 10);
        target.angle           = strList[5].toDouble(nullptr);
        target.start.longitude = strList[1].toDouble(nullptr);  // x1
        target.start.latitude  = strList[2].toDouble(nullptr);  // y1
        target.end.longitude   = strList[3].toDouble(nullptr);  // x2
        target.end.latitude    = strList[4].toDouble(nullptr);  // y2

        auto &x1 = target.start.longitude;
        auto &y1 = target.start.latitude;
        auto &x2 = target.end.longitude;
        auto &y2 = target.end.latitude;

        target.A = y2 - y1;
        target.B = -(x2 - x1);
        target.C = y1 * (x2 - x1) - x1 * (y2 - y1);

        target.key.append(strList[1].toDouble(nullptr));
        target.key.append(strList[3].toDouble(nullptr));

        target.value.append(strList[2].toDouble(nullptr));
        target.value.append(strList[4].toDouble(nullptr));
        targetGraph.append(target);
    }

    return 0;
}

int Navigation::setPostion(const BspConfig::Gps_Info &gps)
{
    auto distance = distanceBetweenPoints(gps.longitude, gps.latitude, prevGpsInfo.longitude, prevGpsInfo.latitude);
    auto time     = gps.current_week_ms - prevGpsInfo.current_week_ms;  // unit : s
    currentSpeed  = (distance / 1000) / (time / 3600);                  // km/h

    //    ui->label_info->setText(QString("当前速度: %1 km/h").arg(currentSpeed));
    showGpsInfo(gps);
    showSystemInfo(currentSpeed);

    historyTracker.key.append(gps.longitude);
    historyTracker.value.append(gps.latitude);

    if(realTimeTracker.key.size() > 10)
    {
        realTimeTracker.key.removeFirst();
        realTimeTracker.value.removeFirst();
    }
    realTimeTracker.key.append(gps.longitude);
    realTimeTracker.value.append(gps.latitude);

    ui->navigationPlot->graph(0)->setPen(QPen(Qt::red, scanWidth(gps.height, 30)));
    ui->navigationPlot->graph(0)->setData(realTimeTracker.key, realTimeTracker.value);
    //    ui->navigationPlot->rescaleAxes();
    ui->navigationPlot->replot();
    prevGpsInfo = gps;
    return 0;
}

/**
 * @brief Navigation::distanceBetweenPoints
 * @param longitude1
 * @param latitude1
 * @param longitude2
 * @param latitude2
 * @return unit: m
 */
double Navigation::distanceBetweenPoints(double longitude1, double latitude1, double longitude2, double latitude2)
{
    auto rad = [](double angle) -> double {
        return angle * 3.14159265 / 180;
    };

    double a = rad(longitude1) - rad(longitude2);
    double b = rad(latitude1) - rad(latitude2);

    double s = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(rad(latitude1)) * cos(rad(latitude2)) * pow(sin(b / 2), 2)));

    s *= 6378.137;
    s *= 1000;
    return s;
}

/**
 * @brief 根据飞行高度以及扫描角度，计算出曲线的线宽
 * 按照 海洋雷达扫描角度：30°， 飞行高度：400m，则 扫描宽度：461米，需要线宽？
 *
 * @param height
 * @param angle
 * @return
 */
double Navigation::scanWidth(double height, double angle)
{
    double rad   = angle * 3.14159265 / 180;
    double width = tan(rad) * 2 * height;

    return 10;
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

        setPostion(gps);
        Common::sleepWithoutBlock(50);

        qDebug() << "gps.longitude = " << gps.longitude
                 << "gps.latitude = " << gps.latitude;
    }
#endif
    return 0;
}
