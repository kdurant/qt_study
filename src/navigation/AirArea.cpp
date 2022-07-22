#include "AirArea.h"
#include <qpoint.h>
#include <qvector.h>
#include <algorithm>
#include <iterator>
#include <tuple>

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

        m_surverArea.airLine.append(AirLine{
            lng_start,
            lat_start,
            lng_end,
            lat_end,
            height, 200});
    }

    auto max_lng = std::max_element(std::begin(lng_set), std::end(lng_set));
    auto min_lng = std::min_element(std::begin(lng_set), std::end(lng_set));
    auto max_lat = std::max_element(std::begin(lat_set), std::end(lat_set));
    auto min_lat = std::min_element(std::begin(lat_set), std::end(lat_set));
    m_surverArea.rect.setTopLeft(QPointF(*min_lng, *min_lat));
    m_surverArea.rect.setBottomRight(QPointF(*max_lng, *max_lat));

    return 0;
}

int AirArea::_getPosOnWhichLine()
{
    int len = m_surverArea.airLine.size();
    for(int i = 0; i < len; i++)
    {
        double distance = gps_distance(m_currentPos.longitude, m_currentPos.latitude, m_surverArea.airLine[i].start.x(), m_surverArea.airLine[i].start.y());
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
        lng_start = this->m_surverArea.airLine[i].start.x();
        lat_start = this->m_surverArea.airLine[i].start.y();
        lng_end   = this->m_surverArea.airLine[i].start.x();
        lat_end   = this->m_surverArea.airLine[i].start.y();

        delta_lng    = lng_end - lng_start;
        delta_lat    = lat_end - lat_start;
        interval_lng = delta_lng / nums;
        interval_lat = delta_lat / nums;

        for(int step = 0; step < nums; step++)
        {
            m_splited_area.append(QPointF(m_surverArea.airLine[i].start.x(), m_surverArea.airLine[i].start.y()) + QPointF(interval_lng, interval_lat) * step);
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

void AirArea::initSurveyPoints(int interval)
{
    m_surverArea.totalValidEle = 0;
    double step                = interval * METER2LNG_LAT;
    int    x_step              = m_surverArea.rect.size().width() / step;
    int    y_step              = m_surverArea.rect.size().height() / step;

    QVector<SurveyPoint> line;
    for(int i = 0; i < y_step; i++)
    {
        for(int j = 0; j < x_step; j++)
        {
            SurveyPoint p;
            p.pos      = m_surverArea.rect.topLeft() + QPointF(i * step, j * step);
            p.valid    = 0;
            p.surveyed = false;
            line.append(p);
        }
        m_surverArea.points.append(line);
    }
}

void AirArea::setSurverPoints()
{
    int len = getAirLineNum();
    for(int i = 0; i < len; i++)
    {
        // 1. 对规划航线进行插值
        QVector<LinePara> virtualLine = interpolateAirLine(m_surverArea.airLine[i]);

        int row = m_surverArea.points.size();
        int col = m_surverArea.points[0].size();

        //        for(int m = 0; m <
    }
    return;
}

double AirArea::point2line_distance(QPointF point, LinePara &line)
{
    double numerator   = abs(line.slope * point.x() - point.y() + line.intercept);  // 分子
    double denominator = sqrt(line.slope * line.slope + (-1 * -1));                 // 分母
    double ret         = numerator / denominator;
    return ret;
}

AirArea::LinePara AirArea::getLinePara(QPointF &p1, QPointF &p2)
{
    LinePara line;
    if(p2.x() - p1.x() == 0)
    {
        line.slope     = 10000;
        line.intercept = p1.y() - line.slope * p1.x();
    }
    else
    {
        line.slope     = (p2.y() - p1.y()) / (p2.x() - p1.x());
        line.intercept = p1.y() - line.slope * p1.x();
    }
    line.start = p1;
    line.end   = p2;
    return line;
}

AirArea::LinePara AirArea::shiftLine(LinePara &line, double verticalDistance)
{
    verticalDistance = verticalDistance * METER2LNG_LAT;  // 距离要转换成经维度单位

    // s1 = kx + b + m/(cos(atan(k))
    LinePara ret;
    ret.slope     = line.slope;
    ret.intercept = line.intercept + verticalDistance / (cos(atan(line.slope)));

    // P2.x = P1.x + m*sin(atan(k)); P2.y = P1.y - m*cos(atan(k))
    double x, y;
    x         = line.start.x() + verticalDistance * sin(atan(line.slope));
    y         = line.start.y() - verticalDistance * cos(atan(line.slope));
    ret.start = QPointF(x, y);

    x       = line.end.x() + verticalDistance * sin(atan(line.slope));
    y       = line.end.y() - verticalDistance * cos(atan(line.slope));
    ret.end = QPointF(x, y);

    return ret;
}

QVector<AirArea::LinePara> AirArea::interpolateAirLine(AirLine &airLine)
{
    QVector<AirArea::LinePara> ret;

    double   width = getScanWidth(airLine.height, m_scanAngle);  // 1. 获得雷达扫描宽度，可知需要新增多少条虚拟航线
    int      num   = width / m_matrixSize;
    LinePara line  = getLinePara(airLine.start, airLine.end);  // 2. 获得规划航线的参数

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
