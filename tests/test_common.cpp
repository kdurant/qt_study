#include <gtest/gtest.h>
#include "common.h"

#include <QtCore>

TEST(test_ba2int, big_endian)
{
    QByteArray ba;
    ba.resize(4);
    ba[0] = 0x12;
    ba[1] = 0x34;
    ba[2] = 0x56;
    ba[3] = 0x78;
    EXPECT_EQ(Common::ba2int(ba, 1), 0x12345678);
}

TEST(test_ba2int, little_endian)
{
    QByteArray ba;
    ba.resize(4);
    ba[0] = 0x12;
    ba[1] = 0x34;
    ba[2] = 0x56;
    ba[3] = 0x78;
    EXPECT_EQ(Common::ba2int(ba, 0), 0x78563412);
}

int main(void)
{
    testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}
