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
void getLaserLineInLaserCoord(double* points, double height, double angle)
{
    points[0] = atan(angle) * height;
    points[1] = 0;
    points[2] = -height;

    points[3] = -atan(angle) * height;
    points[4] = 0;
    points[5] = -height;
}

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
void ComputeLaserToIMUCoord(double* laserCoord, double oX, double oY, double oZ, double detaX, double detaY, double detaZ)
{
    double tempCoord[3];
    tempCoord[0] = (cos(oY) * cos(oZ) - sin(oY) * sin(oX) * sin(oZ)) * laserCoord[0] + cos(oX) * sin(oZ) * laserCoord[1] +
                   (sin(oY) * cos(oZ) + cos(oY) * sin(oX) * sin(oZ)) * laserCoord[2] + detaX;
    tempCoord[1] = (-cos(oY) * sin(oZ) - sin(oY) * sin(oX) * cos(oZ)) * laserCoord[0] + cos(oX) * cos(oZ) * laserCoord[1] +
                   (-sin(oY) * sin(oZ) + cos(oY) * sin(oX) * cos(oZ)) * laserCoord[2] + detaY;
    tempCoord[2] = -sin(oY) * cos(oX) * laserCoord[0] - sin(oX) * laserCoord[1] + cos(oY) * cos(oX) * laserCoord[2] + detaZ;

    laserCoord[0] = tempCoord[0];
    laserCoord[1] = tempCoord[1];
    laserCoord[2] = tempCoord[2];
}

/**
 * @brief
 *
 * @param IMUCoord  输入, 雷达扫描线段两个端点的惯导坐标系坐标
 *                  输出，雷达扫描线段两个端点的当地水平坐标系坐标
 * @param r 翻滚角
 * @param p 俯仰角
 * @param y 偏航角
 */
void CoordCompute::ComputeIMUToLocalCoord(double* IMUCoord, double r, double p, double y)
{
    auto angleToARC = [](double angle)
    {
        return angle * 3.1415926535897932 / 180;
    };

    r = angleToARC * r;  //角度换弧度
    p = angleToARC * p;
    y = angleToARC * y;

    double cr = cos(r);
    double cy = cos(y);
    double tempCoord[3];
    double sr    = sin(r);
    double sp    = sin(p);
    double sy    = sin(y);
    double cp    = cos(p);
    tempCoord[0] = (cr * cy + sr * sp * sy) * IMUCoord[0] + cp * sy * IMUCoord[1] + (sr * cy - cr * sp * sy) * IMUCoord[2];
    tempCoord[1] = (-cr * sy + sr * sp * cy) * IMUCoord[0] + cp * cy * IMUCoord[1] + (-sr * sy - cr * sp * cy) * IMUCoord[2];
    tempCoord[2] = -sr * cp * IMUCoord[0] + sp * IMUCoord[1] + cr * cp * IMUCoord[2];

    IMUCoord[0] = tempCoord[0];
    IMUCoord[1] = tempCoord[1];
    IMUCoord[2] = tempCoord[2];
}

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
void ComputeLocalToWGS84Coord(double* LocalCoord, double B, double L, double H, double xWGS84, double yWGS84, double zWGS84)
{
    B         = angleToARC * B;  //角度换弧度
    L         = angleToARC * L;
    double sr = sin(L);
    double sb = sin(B);
    double cl = cos(L);
    double cb = cos(B);

    double tempCoord[3];
    tempCoord[0] = -sr * LocalCoord[0] - sb * cl * LocalCoord[1] + cb * cl * LocalCoord[2] + xWGS84;
    tempCoord[1] = cl * LocalCoord[0] - sb * sr * LocalCoord[1] + cb * sr * LocalCoord[2] + yWGS84;
    tempCoord[2] = cb * LocalCoord[1] + sb * LocalCoord[2] + zWGS84;

    LocalCoord[0] = tempCoord[0];
    LocalCoord[1] = tempCoord[1];
    LocalCoord[2] = tempCoord[2];
}

/**
 * @brief 将大地坐标BLH转换为空间直角坐标XYZ
 *
 * @param BLH 大地坐标系坐标, 纬度BLH[0]（单位：度），经度BLH[1]（单位：度），大地高BLH[2]（单位：米）
 * @param XYZ 空间直角坐标系坐标, 单位：米
 */
void BLH2XYZ(double* BLH, double* XYZ)
{
    double       w, N;
    double       TWO_PI = 2 * 3.1415926535897932;
    const double A      = 6378137;

    const double F = 1 / 298.257223563;

    const double E2 = F * (2 - F);

    double B = BLH[0] * TWO_PI / 360;  //角度换弧度
    double L = BLH[1] * TWO_PI / 360;

    w = sqrt(1 - E2 * sin(B) * sin(B));
    N = A / w;

    XYZ[0] = (N + BLH[2]) * cos(B) * cos(L);
    XYZ[1] = (N + BLH[2]) * cos(B) * sin(L);
    XYZ[2] = sin(B) * (N * (1 - E2) + BLH[2]);
}

/**
 * @brief 将空间直角坐标转换为大地坐标
 *
 * @param XYZ 已知空间直角坐标，单位：米
 * @param BLH 转换后的大地坐标，纬度BLH[0]（单位：度），经度BLH[1]（单位：度），大地高BLH[2]（单位：米）
 */
void XYZBLH(double* XYZ, double* BLH)
{
    double p, slat, N, htold, latold;

    double TWO_PI = 2 * 3.1415926535897932;
    double A      = 6378137;
    double F      = 1 / 298.257223563;
    double E2     = F * (2 - F);

    p = sqrt(XYZ[0] * XYZ[0] + XYZ[1] * XYZ[1]);
    if(p < 1e-3 / 5)  //位于两极或原点;
    {
        BLH[0] = (XYZ[2] > 0 ? 90.0 : -90.0);    //纬度;
        BLH[1] = 0;                              //经度;
        BLH[2] = fabs(XYZ[2]) - A * (1.0 - E2);  //大地高;
        return;
    }

    BLH[0] = atan2(XYZ[2], p * (1.0 - E2));  //设置初值;
    BLH[2] = 0;
    for(int i = 0; i < 5; i++)
    {
        slat = sin(BLH[0]);
        N    = A / sqrt(1.0 - E2 * slat * slat);  //卯酉圈曲率半径;

        htold  = BLH[2];
        BLH[2] = p / cos(BLH[0]) - N;

        latold = BLH[0];
        BLH[0] = atan2(XYZ[2], p * (1.0 - E2 * (N / (N + BLH[2]))));

        if(fabs(BLH[0] - latold) < 1.0e-9 && fabs(BLH[2] - htold) < 1.0e-9 * A)
        {
            break;
        }
    }

    BLH[1] = atan2(XYZ[1], XYZ[0]);

    if(BLH[1] < 0.0)
    {
        BLH[1] += TWO_PI;
    }
    // BLH[0] *= RAD_TO_DEG;
    // BLH[1] *= RAD_TO_DEG;
}

int main(void)
{
    double height = 200;
    double angle  = 30;

    double laserCoord[6];                                 // 两个端点的6个坐标
    getLaserLineInLaserCoord(laserCoord, height, angle);  // 计算在以雷达为中心的空间直角坐标系中，雷达扫描线段的两个端点坐标

    // 以下流程运行两次
    ComputeLaserToIMUCoord(&laserCoord[0], 0, 0, 0, 0, 0, 0);
    double roll, pitch, heading;
    ComputeIMUToLocalCoord(&laserCoord[0], roll, pitch, heading);

    double xyz[3];
    double longitude, latitude;
    BLH2XYZ(&laserCoord[0], xyz);
    ComputeLocalToWGS84Coord(&laserCoord[0], latitude, longitude, height, &xyz[0], &xyz[1], &xyz[2]);
    XYZBLH(xyz, &laserCoord[0]);

    return 0;
}
