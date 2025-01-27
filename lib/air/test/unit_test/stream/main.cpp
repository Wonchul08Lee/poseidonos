
#include <stdio.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "stream_test.h"

#include "src/config/ConfigParser.cpp"

TEST_F(StreamTest, SendPacket)
{
    stream->SendPacket();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
