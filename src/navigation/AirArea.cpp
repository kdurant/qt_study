#include "AirArea.h"
#include <qthread.h>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <iostream>

void AirArea::setFile(QString &file)
{
    this->m_file = file;
}

int AirArea::parseFile()
{
    QFile file(this->m_file);
    file.open(QIODevice::ReadOnly);

    m_surverArea.airLine.clear();

    auto gps_dfm2decmal = [](QByteArray &gps_dfm) -> double
    {
        QByteArray sep_d;
        sep_d.append(0xc2);
        sep_d.append(0xb0);
        QByteArray sep_m;
        sep_m.append(0x27);
        QByteArray sep_s;
        sep_s.append(0x22);

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

    QVector<double> lng_set;
    QVector<double> lat_set;

    while(!file.atEnd())
    {
        QByteArray        line = file.readLine();
        QList<QByteArray> list = line.split(',');

        double height;
        double lng_start = line.split(',')[1].toDouble(nullptr);  //  gps_dfm2decmal(line.split(',')[1]);
        double lat_start = line.split(',')[2].toDouble(nullptr);  // gps_dfm2decmal(line.split(',')[2]);

        //        track.append(QPointF(lng, lat));
        double lng_end = line.split(',')[3].toDouble(nullptr);  // gps_dfm2decmal(line.split(',')[3]);
        double lat_end = line.split(',')[4].toDouble(nullptr);  // gps_dfm2decmal(line.split(',')[4]);

        lng_set.append(lng_start);
        lng_set.append(lng_end);
        lat_set.append(lat_start);
        lat_set.append(lat_end);

        height = line.split(',')[5].toDouble(nullptr);

        AirLine temp;
        temp.line   = QLineF(QPointF(lng_start, lat_start), QPointF(lng_end, lat_end));
        temp.height = height;
        temp.speed  = 200;
        m_surverArea.airLine.append(temp);
    }

    auto max_lng = std::max_element(std::begin(lng_set), std::end(lng_set));
    auto min_lng = std::min_element(std::begin(lng_set), std::end(lng_set));
    auto max_lat = std::max_element(std::begin(lat_set), std::end(lat_set));
    auto min_lat = std::min_element(std::begin(lat_set), std::end(lat_set));
    m_surverArea.rect.setTopLeft(QPointF(*min_lng, *min_lat));
    m_surverArea.rect.setBottomRight(QPointF(*max_lng, *max_lat));

    return 0;
}

void AirArea::setCurrentPos(BspConfig::Gps_Info pos)
{
    m_currentPos = pos;

    // 计算速度
    _getCurrentSpeed(pos);
    // 覆盖率相关计算
    isPosInDesigned(COVERAGE_THRESHOLD);
    _getCoveragePercent();
    __getCoveragePercent();

    // 当前所在航线计算
    _getPosOnWhichLine();
    m_prevPos = pos;
}

int AirArea::_getPosOnWhichLine()
{
    int len = m_surverArea.airLine.size();
    for(int i = 0; i < len; i++)
    {
        double distance = gps_distance(m_currentPos.longitude, m_currentPos.latitude, m_surverArea.airLine[i].line.p1().x(), m_surverArea.airLine[i].line.p1().y());
        if(distance <= AIRLINE_THRESHOLD)
        {
            m_posOnWhichLine = i + 1;
            return i + 1;
        }
    }
    return m_posOnWhichLine;
}

double AirArea::_getCurrentSpeed(BspConfig::Gps_Info cur)
{
    double distance = gps_distance(cur.longitude, cur.latitude, m_prevPos.longitude, m_prevPos.latitude);
    m_currentSpeed  = 3.6 * distance / 0.2;
    return m_currentSpeed;
}

int AirArea::splitArea(int nums)
{
    int len = this->m_surverArea.airLine.size();

    double lng_start    = 0;
    double lng_end      = 0;
    double lat_start    = 0;
    double lat_end      = 0;
    double delta_lng    = 0;
    double delta_lat    = 0;
    double interval_lng = 0;
    double interval_lat = 0;

    for(int i = 0; i < len; i++)
    {
        lng_start = this->m_surverArea.airLine[i].line.p1().x();
        lat_start = this->m_surverArea.airLine[i].line.p1().y();
        lng_end   = this->m_surverArea.airLine[i].line.p2().x();
        lat_end   = this->m_surverArea.airLine[i].line.p2().y();

        delta_lng    = lng_end - lng_start;
        delta_lat    = lat_end - lat_start;
        interval_lng = delta_lng / nums;
        interval_lat = delta_lat / nums;

        for(int step = 0; step < nums; step++)
        {
            m_splited_area.append(QPointF(m_surverArea.airLine[i].line.p1().x(), m_surverArea.airLine[i].line.p1().y()) + QPointF(interval_lng, interval_lat) * step);
        }
    }

    m_coveragePoints = QVector<int>(m_splited_area.length(), 0);
    return 0;
}

int AirArea::isPosInDesigned(double r)
{
    int len = m_splited_area.length();
    if(len == 0)
        return -1;

    /**
     * 对于某一个飞机位置，遍历所有规划航线轨迹点的时候，可能有不止一个点认为这个位置
     * 是在自己的范围之内，所以一个航迹点对应的计数值可能会大于1
     */
    for(int i = 0; i < len; i++)
    {
        double distance = gps_distance(m_currentPos.longitude, m_currentPos.latitude, m_splited_area[i].x(), m_splited_area[i].y());
        if(distance < r)
        {
            m_coveragePoints[i]++;
        }
    }

    return 0;
}

double AirArea::_getCoveragePercent()
{
    int covered{0};

    int len = m_coveragePoints.length();
    for(int i = 0; i < len; i++)
    {
        if(m_coveragePoints[i] > 0)
            covered++;
    }
    m_coveragePercent = static_cast<double>(covered) / len;
    return m_coveragePercent;
}

double AirArea::__getCoveragePercent()
{
    QVector<BspConfig::Gps_Info> points = interpolateScanPoint(m_prevPos, m_currentPos);

    for(auto &p : points)
    {
        AirLine temp = getRadarScanExpression(p);
        calcRealSurvey(temp);
    }
    m_surverArea.surveyedPercent = m_surverArea.hasSurveyedEle / m_surverArea.totalValidEle;
    return m_surverArea.surveyedPercent;
}

AirArea::AirLine AirArea::getRadarScanExpression(BspConfig::Gps_Info &pos)
{
    // AirLine airway;

    // return airway;
    return m_surverArea.airLine[0];
}

void AirArea::calcRealSurvey(AirLine &line)
{
    int _row = m_surverArea.points.size();
    int _col = m_surverArea.points[0].size();

    for(int row = 0; row < _row; row++)
    {
        for(int col = 0; col < _col; col++)
        {
            if(m_surverArea.points[row][col].marker > 0)
            {
                double distance = point2seg_distance(m_surverArea.points[row][col].pos, line.line);
                if(distance <= COVERAGE_THRESHOLD)
                {
                    m_surverArea.points[row][col].surveyed++;
                }
            }
        }
    }

    for(int row = 0; row < _row; row++)
    {
        for(int col = 0; col < _col; col++)
        {
            if(m_surverArea.points[row][col].surveyed > 0)
            {
                m_surverArea.hasSurveyedEle++;
            }
        }
    }
}

QVector<BspConfig::Gps_Info> AirArea::interpolateScanPoint(BspConfig::Gps_Info &prev, BspConfig::Gps_Info &cur)
{
    QVector<BspConfig::Gps_Info> track;

    if(prev.week == 0)
        track.append(cur);
    else
    {
        // speed *1000/3600 * 0.2
        // 飞机速度200km/h时， 200ms飞行的距离为：11.11m
        // 飞机速度300km/h时， 200ms飞行的距离为：16.67m

        double distance = m_currentSpeed / 19;
        if(distance < COVERAGE_THRESHOLD)
        {
            track.append(cur);
            return track;
        }
        int num = distance / COVERAGE_THRESHOLD;

        double longitude_step = (cur.longitude - prev.longitude) / num;
        double latitude_step  = (cur.latitude - prev.latitude) / num;
        double height_step    = (cur.height - prev.height) / num;
        double azimuth_step   = (cur.heading - prev.heading) / num;
        double pitch_step     = (cur.pitch - prev.pitch) / num;
        double roll_step      = (cur.roll - prev.roll) / num;

        BspConfig::Gps_Info temp;
        for(int i = 0; i < num; i++)
        {
            temp.longitude = prev.longitude + longitude_step * (i + 1);
            temp.latitude  = prev.latitude + latitude_step * (i + 1);
            temp.height    = prev.height + height_step * (i + 1);
            temp.heading   = prev.heading + azimuth_step * (i + 1);
            temp.pitch     = prev.pitch + pitch_step * (i + 1);
            temp.roll      = prev.roll + roll_step * (i + 1);
            track.append(temp);
        }
    }
    return track;
}

void AirArea::initSurveyArea(int interval)
{
    m_surverArea.totalValidEle = 0;
    m_surverArea.points.clear();

    double step   = interval * METER2LNG_LAT;
    int    x_step = m_surverArea.rect.size().width() / step;
    int    y_step = m_surverArea.rect.size().height() / step;

    for(int i = 0; i < y_step; i++)
    {
        QVector<SurveyPoint> line;
        for(int j = 0; j < x_step; j++)
        {
            SurveyPoint p;
            p.pos      = m_surverArea.rect.topLeft() + QPointF(j * step, i * step);
            p.marker   = 0;
            p.surveyed = false;
            line.append(p);
        }

        m_surverArea.points.append(line);
    }
}

void AirArea::setSurverArea()
{
#if 0
    int row = m_surverArea.points.size();
    int col = m_surverArea.points[0].size();

    for(int m = 0; m < row; m++)
    {
        for(int n = 0; n < col; n++)
        {
            double distance = point2seg_distance(m_surverArea.points[m][n].pos, m_surverArea.airLine[0].line);
            if(distance < COVERAGE_THRESHOLD)
            {
                m_surverArea.points[m][n].valid++;
            }
        }
    }
#else
    int len = getAirLineNum();

    for(int i = 0; i < len; i++)
    {
        // 1. 对规划航线进行插值
        QVector<LinePara> virtualLine = interpolateAirLine(m_surverArea.airLine[i]);

        // 2. 使用插值过的航线标记有效测区
        for(auto &line : virtualLine)
        {
            int __row = m_surverArea.points.size();
            int __col = m_surverArea.points[0].size();

            for(int row = 0; row < __row; row++)
            {
                for(int col = 0; col < __col; col++)
                {
                    double distance = point2seg_distance(m_surverArea.points[row][col].pos, line.line);
                    if(distance < COVERAGE_THRESHOLD)
                        m_surverArea.points[row][col].marker++;
                }
            }
        }
    }
    // 3. 计算有多少有效的测区
    int __row = m_surverArea.points.size();
    int __col = m_surverArea.points[0].size();

    for(int row = 0; row < __row; row++)
    {
        for(int col = 0; col < __col; col++)
        {
            if(m_surverArea.points[row][col].marker > 0)
                m_surverArea.totalValidEle++;
        }
    }
#endif
    return;
}

void AirArea::printSurverArea()
{
    int row = m_surverArea.points.size();
    int col = m_surverArea.points[0].size();

    qDebug() << "printSurverPoints";
    for(int m = 0; m < row; m++)
    {
        for(int n = 0; n < col; n++)
        {
            if(m_surverArea.points[m][n].marker > 0)
            {
                std::cout << '*';
            }
            else
                std::cout << '-';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
    std::fflush(stdout);

    qDebug() << "Surver Matrix: row = " << row
             << "col = " << col
             << "\nm_surverArea.totalValidEle = " << m_surverArea.totalValidEle;
}

double AirArea::point2line_distance(QPointF point, QLineF &line)
{
    double ret;
    double k, b;
    if(line.x1() == line.x2())
        k = 999999;
    else
        k = (line.y2() - line.y1()) / (line.x2() - line.x1());

    b = line.y1() - k * line.x1();

    double numerator   = abs(k * point.x() - point.y() + b);  // 分子
    double denominator = sqrt(k * k + (-1 * -1));             // 分母
    ret                = numerator / denominator;

    return ret;
}

double AirArea::point2seg_distance(QPointF point, QLineF &target)
{
    double distance;
    auto   __line_magnitude = [](QPointF p1, QPointF p2) -> double
    {
        double x1 = (p2.x() - p1.x()) * (p2.x() - p1.x());
        double y1 = (p2.y() - p1.y()) * (p2.y() - p1.y());
        double d1 = sqrt(x1 + y1);
        return d1;
    };
    double line_maginutude = __line_magnitude(target.p1(), target.p2());

    double u1 = (point.x() - target.x1()) * (target.x2() - target.x1()) +
                (point.y() - target.y1()) * (target.y2() - target.y1());
    double u = u1 / (line_maginutude * line_maginutude);

    double ix, iy;
    if(u < 0.00001 || u > 1)
    {
        // 点到直线的投影不在线段内, 计算点到两个端点距离的最小值即为"点到线段最小距离"

        ix = __line_magnitude(point, target.p1());
        iy = __line_magnitude(point, target.p2());

        if(ix > iy)
            distance = iy;
        else
            distance = ix;
    }
    else
    {  // 投影点在线段内部, 计算方式同点到直线距离, u 为投影点距离x1在x1x2上的比例, 以此计算出投影点的坐标
        ix       = target.x1() + u * (target.x2() - target.x1());
        iy       = target.y1() + u * (target.y2() - target.y1());
        distance = __line_magnitude(point, QPointF(ix, iy));
    }
    return distance * 111000;
}

AirArea::LinePara AirArea::getLinePara(QLineF &line)
{
    LinePara ret;
    if(line.p2().x() - line.p1().x() == 0)
    {
        ret.slope     = 10000;
        ret.intercept = line.p1().y() - ret.slope * line.p1().x();
    }
    else
    {
        ret.slope     = (line.p2().y() - line.p1().y()) / (line.p2().x() - line.p1().x());
        ret.intercept = line.p1().y() - ret.slope * line.p1().x();
    }
    ret.line = line;
    return ret;
}

AirArea::LinePara AirArea::shiftLine(LinePara &source_line, double verticalDistance)
{
    verticalDistance = verticalDistance * METER2LNG_LAT;  // 距离要转换成经维度单位

    // s1 = kx + b + m/(cos(atan(k))
    LinePara ret;
    ret.slope     = source_line.slope;
    ret.intercept = source_line.intercept + verticalDistance / (cos(atan(source_line.slope)));

    // P2.x = P1.x + m*sin(atan(k)); P2.y = P1.y - m*cos(atan(k))
    double x, y;
    x = source_line.line.p1().x() + verticalDistance * sin(atan(source_line.slope));
    y = source_line.line.p1().y() - verticalDistance * cos(atan(source_line.slope));
    ret.line.setP1(QPointF(x, y));

    x = source_line.line.p2().x() + verticalDistance * sin(atan(source_line.slope));
    y = source_line.line.p2().y() - verticalDistance * cos(atan(source_line.slope));
    ret.line.setP2(QPointF(x, y));

    return ret;
}

QVector<AirArea::LinePara> AirArea::interpolateAirLine(AirLine &airLine)
{
    QVector<AirArea::LinePara> ret;

    double   width = getScanWidth(airLine.height, m_scanAngle);  // 1. 获得雷达扫描宽度，可知需要新增多少条虚拟航线
    int      num   = width / m_matrixSize;
    LinePara line  = getLinePara(airLine.line);  // 2. 获得规划航线的参数

    ret.append(line);
    for(int i = 1; i < num / 2; i++)
    {
        LinePara temp = shiftLine(line, m_matrixSize * i);
        ret.append(temp);
    }
    for(int i = 1; i < num / 2; i++)
    {
        LinePara temp = shiftLine(line, -m_matrixSize * i);
        ret.append(temp);
    }
    return ret;
}

double AirArea::getScanWidth(double height, double angle)
{
    angle        = angle * M_PI / 180.0;
    double width = tan(angle) * height * 2;
    return width;
}
