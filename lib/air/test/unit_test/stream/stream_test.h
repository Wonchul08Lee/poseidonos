
#include "src/stream/Stream.h"
#include "src/stream/Stream.cpp"

class StreamTest : public ::testing::Test
{
public:
    stream::Stream* stream {nullptr};

protected:
    StreamTest () {
        stream = new stream::Stream {};
    }
    ~StreamTest() override {
        if(nullptr != stream) {
            delete stream;
        }
    }
    void SetUp() override {}
    void TearDown() override {}
};
