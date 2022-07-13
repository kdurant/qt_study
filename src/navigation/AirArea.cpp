#include "AirArea.h"

void AirArea::setFile(QString &file)
{
    this->m_file = file;
}

int AirArea::parseFile()
{
    QFile file(this->m_file);
    file.open(QIODevice::ReadOnly);

    m_line.clear();

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

        height = line.split(',')[5].toDouble(nullptr);

        this->m_line.append(AirLine{
            lng_start,
            lat_start,
            lng_end,
            lat_end,
            height, 200});
    }

    return 0;
}

int AirArea::_getPosOnWhichLine()
{
    int len = m_line.size();
    for(int i = 0; i < len; i++)
    {
        double distance = gps_distance(m_currentPos.longitude, m_currentPos.latitude, m_line[i].start.x(), m_line[i].start.y());
        if(distance <= THRESHOLD)
        {
            m_posOnWhichLine = i + 1;
            return i + 1;
        }
    }
    m_posOnWhichLine = -1;
    return -1;
}

double AirArea::getCurrentSpeed(BspConfig::Gps_Info cur)
{
    double distance = gps_distance(cur.longitude, cur.latitude, m_prevPos.latitude, m_prevPos.longitude);
    m_currentSpeed  = 3.6 * distance / 1;
    return m_currentSpeed;
}

int AirArea::splitArea(int nums)
{
    int len = this->m_line.size();

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
        lng_start = this->m_line[i].start.x();
        lat_start = this->m_line[i].start.y();
        lng_end   = this->m_line[i].start.x();
        lat_end   = this->m_line[i].start.y();

        delta_lng    = lng_end - lng_start;
        delta_lat    = lat_end - lat_start;
        interval_lng = delta_lng / nums;
        interval_lat = delta_lat / nums;

        for(int step = 0; step < nums; step++)
        {
            m_splited_area.append(QPointF(this->m_line[i].start.x(), this->m_line[i].start.y()) + QPointF(interval_lng, interval_lat) * step);
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
