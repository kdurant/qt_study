#include <gtest/gtest.h>
#include <iostream>
#include "AirArea.h"

#include <QtCore>

AirArea area;
TEST(test_gps_distance, big_endian)
{
    double lng1 = 121.232;
    double lat1 = 25.0564;
    double lng2 = 121.2277;
    double lat2 = 25.0542;
    double d    = area.gps_distance(lng1, lat1, lng2, lat2);  // 497.14
    std::cout << d << std::endl;
    EXPECT_EQ(static_cast<int>(d), 497);
}

int main(void)
{
    testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}

