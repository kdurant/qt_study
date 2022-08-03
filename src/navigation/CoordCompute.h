#include <tuple>
class CoordCompute
{
public:
    CoordCompute() = default;

    std::tuple<double, double, double, double>
    getLaserLineEndPoints(double scan_angle,
                          double longitude, double latitude, double height,
                          double roll, double pitch, double heading)
    {
        double BLH[3]   = {latitude, longitude, height};
        double Txyz1[3] = {0};

        GetLaserLineCoord(laserCoord, height, scan_angle);

        ComputeLaserToIMUCoord(&laserCoord[0], 0, 0, 0, 0, 0, 0);
        // roll 和 pitch 交换是由于飞机安装的原因
        ComputeIMUToLocalCoord(&laserCoord[0], pitch, -roll, heading + 90);
        BLH2XYZ(BLH, xyz);
        ComputeLocalToWGS84Coord(&laserCoord[0], latitude, longitude, height, xyz[0], xyz[1], xyz[2]);
        XYZBLH(&laserCoord[0], Txyz1);
        endpoints[0] = Txyz1[0] * 180 / 3.141592653;
        endpoints[1] = Txyz1[1] * 180 / 3.141592653;
        endpoints[2] = Txyz1[2];

        ComputeLaserToIMUCoord(&laserCoord[3], 0, 0, 0, 0, 0, 0);
        ComputeIMUToLocalCoord(&laserCoord[3], pitch, -roll, heading + 90);
        BLH2XYZ(BLH, xyz);
        ComputeLocalToWGS84Coord(&laserCoord[3], latitude, longitude, height, xyz[0], xyz[1], xyz[2]);
        XYZBLH(&laserCoord[3], Txyz1);
        endpoints[3] = Txyz1[0] * 180 / 3.141592653;
        endpoints[4] = Txyz1[1] * 180 / 3.141592653;
        endpoints[5] = Txyz1[2];

        return {endpoints[1], endpoints[0], endpoints[4], endpoints[3]};
    }

private:
    double laserCoord[6] = {0};
    double xyz[3]        = {0};
    double endpoints[6];

    double angleToARC(double angle)
    {
        return angle * 3.1415926535897932 / 180;
    };

    /**
     * @brief 计算在以雷达为中心的空间直角坐标系中，雷达扫描线段的两个端点
     * y轴方向为0
     *
     * @param points 返回两个端点的坐标
     *                  points[0]: 起点x, points[1]: 起点y, points[2]: 起点z
     *                  points[3]: 终点x, points[4]: 终点y, points[5]: 终点z
     * @param height GPS高度
     * @param angle  雷达扫描角度
     */
    void GetLaserLineCoord(double* points, double height, double angle);

    /**
     * @brief 扫描仪坐标系到惯导坐标系的转换, 如果x，y，z方向一直，可以省略
     *
     * @param laserCoord 用作返回值
     * @param oX X方向旋转（弧度）
     * @param oY Y方向旋转（弧度）
     * @param oZ Z方向旋转（弧度）
     * @param detaX X方向平移（米）
     * @param detaY Y方向平移（米）
     * @param detaZ Z方向平移（米）
     */
    void ComputeLaserToIMUCoord(double* laserCoord, double oX, double oY, double oZ, double detaX, double detaY, double detaZ);

    /**
     * @brief
     *
     * @param IMUCoord  输入, 雷达扫描线段两个端点的惯导坐标系坐标
     *                  输出，雷达扫描线段两个端点的当地水平坐标系坐标
     * @param r 翻滚角
     * @param p 俯仰角
     * @param y 偏航角
     */
    void ComputeIMUToLocalCoord(double* IMUCoord, double r, double p, double y);

    /**
     * @brief 当地水平坐标系到WGS84坐标系
     *
     * @param LocalCoord
     * @param B
     * @param L
     * @param H
     * @param xWGS84
     * @param yWGS84
     * @param zWGS84
     */
    void ComputeLocalToWGS84Coord(double* LocalCoord, double B, double L, double H, double xWGS84, double yWGS84, double zWGS84);

    /**
     * @brief 将大地坐标BLH转换为空间直角坐标XYZ
     *
     * @param BLH 大地坐标系坐标, 纬度BLH[0]（单位：度），经度BLH[1]（单位：度），大地高BLH[2]（单位：米）
     * @param XYZ 空间直角坐标系坐标, 单位：米
     */
    void BLH2XYZ(double* BLH, double* XYZ);

    /**
     * @brief 将空间直角坐标转换为大地坐标
     *
     * @param XYZ 已知空间直角坐标，单位：米
     * @param BLH 转换后的大地坐标，纬度BLH[0]（单位：度），经度BLH[1]（单位：度），大地高BLH[2]（单位：米）
     */
    void XYZBLH(double* XYZ, double* BLH);
};
