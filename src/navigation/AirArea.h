/* =============================================================================
# FileName    :	AirArea.h
# Author      :	author
# Email       :	email@email.com
# Description :	纬度(latitude)，南北方向变化,坐标轴上的y
                经度(longitude)，东西方向变化, 坐标轴上的x
                google Map里先显示纬度，再显示经度

                1经度或者1纬度的变化，大概是111km

经度（东西方向）1M实际度：360°/31544206M=1.141255544679108e-5=0.00001141
纬度（南北方向）1M实际度：360°/40030173M=8.993216192195822e-6=0.00000899

# Version     :	1.0
# LastChange  :	2022-07-21 13:32:08
# ChangeLog   :
============================================================================= */
#ifndef AIRAREA_H
#define AIRAREA_H
#include <QtCore>
#include "bsp_config.h"

class AirArea : public QObject
{
    // Q_OBJECT
public:
    struct AirLine  // 单条航线
    {
        QLineF line;
        double height;  // unit: m
        double speed;   // unit: km/h

        AirLine(double lng_start, double lat_start, double lng_end, double lat_end, double height, double speed)
        {
            line.setP1(QPointF(lng_start, lat_start));
            line.setP2(QPointF(lng_end, lat_end));
            this->height = height;
            this->speed  = speed;
        }
    };

    struct SurveyPoint
    {
        QPointF pos;
        int     valid;  // 大于1, 有效的测区标记
        bool    surveyed;
    };

    struct SurveyArea
    {
        int                           totalValidEle;
        QRectF                        rect;
        QVector<AirLine>              airLine;  // 规划的航线
        QVector<QVector<SurveyPoint>> points;   // 测区计算需要的点
    };

    struct LinePara
    {
        double slope;
        double intercept;
        QLineF line;
    };

public:
    AirArea() :
        m_currentSpeed(0),
        m_posOnWhichLine(-1),
        m_coveragePercent(0)
    {
        m_coveragePoints.clear();
        m_splited_area.clear();
        m_surverArea.airLine.clear();
    };

    /**
     * @brief gps_distance
     * @param lng1
     * @param lat1
     * @param lng2
     * @param lat2
     * @return distance, unit: m
     */
    double gps_distance(double lng1, double lat1, double lng2, double lat2)
    {
        auto radians = [](double d)
        {
            return d * M_PI / 180.0;
        };

        double radLng1 = radians(lng1);
        double radLat1 = radians(lat1);
        double radLng2 = radians(lng2);
        double radLat2 = radians(lat2);

        double a = radLat1 - radLat2;
        double b = radLng1 - radLng2;

        double d = 2 * asin(sqrt(sin(a / 2) * sin(a / 2) + cos(radLat1) * cos(radLat2) * sin(b / 2) * sin(b / 2))) * 6371000;
        // double d = 2 * asin(sqrt(sin(a / 2) * sin(a / 2) + cos(radLat1) * cos(radLat2) * sin(b / 2) * sin(b / 2))) * 6378137;
        return d;
    }

    /**
     * @brief 设置规划航迹文件
     *
     * @param file
     */
    void setFile(QString& file);
    /**
     * @brief 分析航迹文件，得到具体航线信息
     *      先清除以前保存的航迹信息m_line
     * @return
     */
    int parseFile(void);

    void setCurrentPos(BspConfig::Gps_Info pos)
    {
        m_currentPos = pos;

        // 计算速度
        _getCurrentSpeed(pos);
        m_prevPos = pos;
        // 覆盖率相关计算
        isPosInDesigned(COVERAGE_THRESHOLD);
        _getCoveragePercent();
        // 当前所在航线计算
        _getPosOnWhichLine();
    }

    /**
     * @brief 当前位置在那条规划的航迹上
     * 当前GPS位置与航迹覆盖率测试分割的点进行距离计算，小于10m，则认为属于当前航迹
     */
    int _getPosOnWhichLine(void);
    int getPosOnWhichLine(void)
    {
        return m_posOnWhichLine;
    }

    double _getCurrentSpeed(BspConfig::Gps_Info cur);

    double getCurrentSpeed(void)
    {
        return m_currentSpeed;
    }

    /**
     * @brief 将规划的轨迹分割成点的集合，计算测区覆盖率使用
     * @param nums 每条航线将会被分割成的点数

     * @return m_splited_area也会获得此结果
     */
    int splitArea(int nums);

    /**
     * @brief 遍历规划航迹之内所有点，判断当前的GPS位置是否在
     *   初始化 m_coveragePoints
     * @param 当前GPS位置的半径，模拟GPS高度
     * @return
     */
    int isPosInDesigned(double r);

    double _getCoveragePercent();
    double getCoveragePercent()
    {
        return m_coveragePercent;
    }

    /**
     * @brief 当前加载的航迹文件有几条航线
     *
     * @return
     */
    int getAirLineNum(void)
    {
        return m_surverArea.airLine.size();
    }
    AirLine getAirLine(int num)
    {
        return m_surverArea.airLine[num];
    }

    double getHeightDeviation()
    {
        if(m_posOnWhichLine < 0)
            return -10001;

        double expect_height = m_surverArea.airLine[m_posOnWhichLine].height;
        return m_currentPos.height - expect_height;
    }

    double getSpeedDeviation()
    {
        if(m_posOnWhichLine < 0)
            return -10001;

        double expect = m_surverArea.airLine[m_posOnWhichLine].speed;
        return m_currentSpeed - expect;
    }
    double getAzimutuDeriation()
    {
        return -10001;
    }

    /**
     * @brief, 将测区的矩形区域划分为矩阵, 并初始化
     * @param interval unit:m
     */
    void initSurveyPoints(int interval);
    /**
     * @brief 根据航迹，初始化矩阵的内容
     * 要对已有航线进行拟合扩展，使用拟合后的航线集对矩阵元素进行标记
     * 设：航线轨迹方程 s = kx+b, 起点为P1，终点为P2
     * 在垂直航迹的方向上平移 m
     * 新的轨迹方程 s1 = kx + b + m/(cos(atan(k))
     * 新轨迹方程的起点：P2.x = P1.x + m*sin(atan(k)); P2.y = P1.y - m*cos(atan(k))
     */
    void setSurverPoints(void);

    /**
     * @brief 点到直线的垂直距离
     * @param point
     * @param line
     * @return
     */
    double point2line_distance(QPointF point, QLineF& line);

    /**
     * @brief 获得航线在坐标系中的数学表达式
     * @return
     */
    LinePara getLinePara(QLineF& line);

    /**
     * @brief 将给定的直线方程进行平移，得到新的直线
     * @param verticalDistance， 直线垂直方向平移的距离
     * @return
     */
    LinePara shiftLine(LinePara& line, double verticalDistance);

    /**
     * @brief 根据航高计算出的扫描宽度，以及矩阵元素大小，对预设航线进行插值，然后和矩阵比对，标记有效测区
     * 例如扫描宽度为100米，矩阵元素大小为1米，则在预设航线两边各插值50条航线
     * 应该保存航线的数学表达式
     *
     * 1. 计算原航线的数学表达式
     * 2. 计算插值航线的数学表达式，以及新航线的两个端点
     * @param airLine, 规划的航线
     * @return
     */
    QVector<LinePara> interpolateAirLine(AirLine& airLine);

    /**
     * @brief 计算雷达当前扫描区域的宽度
     * @param height
     * @param angle
     * @return
     */
    double getScanWidth(double height, double angle);

private:
    double              AIRLINE_THRESHOLD{20.0};
    double              COVERAGE_THRESHOLD{20.0};
    double              METER2LNG_LAT{0.0000102};
    QString             m_file;
    BspConfig::Gps_Info m_prevPos{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    BspConfig::Gps_Info m_currentPos{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    SurveyArea m_surverArea;

    double m_scanAngle{30};   // 飞机雷达的扫描角度；海洋雷达30度，陆地雷达60度
    double m_matrixSize{10};  // 矩阵元素的大小，单位：米

    double           m_currentSpeed{0};
    int              m_posOnWhichLine{-1};
    double           m_coveragePercent{0};
    QVector<int>     m_coveragePoints;  // QVector的长度和规划航线划分的点个数一致
    QVector<QPointF> m_splited_area;    // 区域内航迹分割的点， 用于覆盖率计算
};
#endif
