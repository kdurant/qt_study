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
        QPointF start;
        QPointF end;
        double  height;  // unit: m
        double  speed;   // unit: km/h

        AirLine(double lng_start, double lat_start, double lng_end, double lat_end, double height, double speed)
        {
            start        = QPointF(lng_start, lat_start);
            end          = QPointF(lng_end, lat_end);
            this->height = height;
            this->speed  = speed;
        }
    };

public:
    AirArea() :
        m_currentSpeed(0),
        m_posOnWhichLine(-1),
        m_coveragePercent(0)
    {
        m_coveragePoints.clear();
        m_splited_area.clear();
        m_line.clear();
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
    int getLinesNum(void)
    {
        return this->m_line.size();
    }
    AirLine getLineInfo(int num)
    {
        return this->m_line[num];
    }

    double getHeightDeviation()
    {
        if(m_posOnWhichLine < 0)
            return -10001;

        double expect_height = m_line[m_posOnWhichLine].height;
        return m_currentPos.height - expect_height;
    }

    double getSpeedDeviation()
    {
        if(m_posOnWhichLine < 0)
            return -10001;

        double expect = m_line[m_posOnWhichLine].speed;
        return m_currentSpeed - expect;
    }
    double getAzimutuDeriation()
    {
        return -10001;
    }

private:
    double              AIRLINE_THRESHOLD{20.0};
    double              COVERAGE_THRESHOLD{20.0};
    QString             m_file;
    BspConfig::Gps_Info m_prevPos{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    BspConfig::Gps_Info m_currentPos{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    QVector<AirLine> m_line;

    double           m_currentSpeed{0};
    int              m_posOnWhichLine{-1};
    double           m_coveragePercent{0};
    QVector<int>     m_coveragePoints;  // QVector的长度和规划航线划分的点个数一致
    QVector<QPointF> m_splited_area;    // 区域内航迹分割的点， 用于覆盖率计算
};
#endif
