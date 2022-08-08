#include "navigation.h"
#include "ui_navigation.h"
#include <cmath>

Navigation::Navigation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Navigation)
{
    ui->setupUi(this);
    timer1s = startTimer(1000);

    initUI();
    initSignalSlot();
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
}

void Navigation::initSignalSlot()
{
    /**
     * 1. 分析航迹. 2. 分割航迹，形成测试点. 3.初始化m_coveragePoints
     */
    connect(ui->btn_loadTracker, &QPushButton::pressed, this, [this]()
            {
        QString trackerFile = QFileDialog::getOpenFileName(this, tr(""), "", tr("*"));  //选择路径
        if(trackerFile.size() == 0)
            return;
        ui->lineEdit_trackerFile->setText(trackerFile);
        m_designedAirArea.setFile(trackerFile);
        m_designedAirArea.parseFile();
        m_designedAirArea.setCoverageThreshold(20);
        m_designedAirArea.initSurveyArea(20);  // COVERAGE_THRESHOLD
        m_designedAirArea.setSurverArea();
        //        m_designedAirArea.printSurverPoints();

        ui->mapView->loadSurveyBorder(m_designedAirArea.getSurveyRect());

        int len = m_designedAirArea.getAirLineNum();
        for(int i = 0; i < len; i += 1)
        {
            AirArea::AirLine line = m_designedAirArea.getAirLine(i);

            ui->mapView->loadTracker(line.line.p1(), line.line.p2(), QPen(Qt::red));
            ui->mapView->loadSerialNum(line.line.p1(), i + 1);
        }
        m_designedAirArea.splitArea(10);

        isLoadedTracker = true;
    });

    connect(ui->btn_loadMap, &QPushButton::pressed, this, [&]
            {
        mapPath = QFileDialog::getExistingDirectory();
        if(mapPath.length() == 0)
            return;
        ui->lineEdit_mapFile->setText(mapPath);
        ui->mapView->deleleAllItems();
        ui->mapView->setMapPath(mapPath);
        ui->mapView->parseMapInfo();

        MapView::TileMapInfo info = ui->mapView->getMapInfo();
        ui->mapView->setDefaultZoom(info.min_zoom_level);
        ui->spinBox_mapMinMapLevel->setValue(info.min_zoom_level);
        ui->spinBox_mapMaxMapLevel->setValue(info.max_zoom_level);
        ui->horizontalSlider_zoomCtrl->setRange(info.min_zoom_level, info.max_zoom_level);
        ui->mapView->loadMap();
        isLoadedMap = true;
    });

    connect(ui->horizontalSlider_zoomCtrl,
            static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged),
            ui->spinBox_mapCurrentZoom,
            &QSpinBox::setValue);

    connect(ui->horizontalSlider_zoomCtrl, &QSlider::valueChanged, this, [&]
            {
        int zoom = ui->horizontalSlider_zoomCtrl->value();

        ui->mapView->deleleAllItems();
        ui->mapView->setDefaultZoom(zoom);
        ui->mapView->loadMap();

        int len = m_designedAirArea.getAirLineNum();
        for(int i = 0; i < len; i += 1)
        {
            AirArea::AirLine temp = m_designedAirArea.getAirLine(i);

            ui->mapView->loadTracker(temp.line.p1(), temp.line.p2(), QPen(Qt::red));
            ui->mapView->loadSerialNum(temp.line.p1(), i + 1);
        }

        for(auto &pos : m_realtime_path)
        {
            ui->mapView->loadRealTimePoint(pos);
        }
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

void Navigation::showSystemInfo(void)
{
    QList<QTreeWidgetItem *> itemList;

    itemList = ui->treeWidget_navigation->findItems("系统参数", Qt::MatchExactly);
    itemList.first()->child(0)->setText(1, QString::number(m_designedAirArea.getCurrentSpeed(), 'g', 6));
    itemList.first()->child(1)->setText(1, QString::number(m_designedAirArea.getCoveragePercent(), 'g', 6));
    itemList.first()->child(2)->setText(1, QString::number(m_designedAirArea.getPosOnWhichLine()));
}

void Navigation::updateGpsInfo(BspConfig::Gps_Info &data)
{
    m_designedAirArea.setCurrentPos(data);
    double currentSpeed = m_designedAirArea.getCurrentSpeed();

    ui->doubleSpinBox_flightHeight->setValue(data.height);
    ui->doubleSpinBox_flightSpeed->setValue(currentSpeed);
    ui->doubleSpinBox_heading->setValue(data.heading);

    ui->label_flightHeight->setText("飞行高度:" + QString::number(data.altitude, 'g', 6));
    ui->label_flightSpeed->setText("飞行速度:" + QString::number(currentSpeed, 'g', 6));
    ui->label_heading->setText("航向角:" + QString::number(data.heading, 'g', 6));

    ui->label_navCoverage->setText("测区覆盖率:" + QString::number(m_designedAirArea.getCoveragePercent(), 'g', 6));

    showGpsInfo(data);
    showSystemInfo();

    if(m_designedAirArea.getHeightDeviation() < -10000)
        ui->label_flightHeightDeviation->setText("nan");
    else
        ui->label_flightHeightDeviation->setText(QString::number(m_designedAirArea.getHeightDeviation(), 'g', 6));

    if(m_designedAirArea.getSpeedDeviation() < -10000)
        ui->label_flightSpeedDeviation->setText("nan");
    else
        ui->label_flightSpeedDeviation->setText(QString::number(m_designedAirArea.getSpeedDeviation(), 'g', 6));

    if(m_designedAirArea.getAzimutuDeriation() < -10000)
        ui->label_headingDeviation->setText("nan");
    else
        ui->label_headingDeviation->setText(QString::number(m_designedAirArea.getAzimutuDeriation(), 'g', 6));

    if(isLoadedMap == false || isLoadedTracker == false)
        return;

    m_realtime_path.append(QPointF(data.longitude, data.latitude));
    ui->mapView->loadRealTimePoint(QPointF(data.longitude, data.latitude));

    if(ui->checkBox_scanLine->isChecked())
    {
        QLineF l = m_designedAirArea.getRadarScanExpression();
        ui->mapView->loadTracker(l.p1(), l.p2(), QPen(Qt::yellow));
    }
}

void Navigation::timerEvent(QTimerEvent *event)
{
    if(timer1s == event->timerId())
    {
        // double percent = m_designedAirArea.getCoveragePercent();
        // ui->label_navCoverage->setText(QString::number(percent, 'g', 6));
    }
}
