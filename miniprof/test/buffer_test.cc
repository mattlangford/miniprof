#include "miniprof/buffer.hh"

#include <gtest/gtest.h>

namespace miniprof {

TEST(Buffer, basic) {
    Buffer::Config config;
    config.buffer_size = 4;

    Buffer buffer{config};
    buffer.push(Entry{"test", 100});
    buffer.push(Entry{"test2", 200});
    buffer.push(Entry{"test3", 300});

    // The buffer should be at maximum capacity now

    {
        std::vector<Entry> output;
        ASSERT_TRUE(buffer.flush_into(output));
        ASSERT_EQ(output.size(), 3);
        EXPECT_EQ(std::string(output.front().name), "test");
        EXPECT_EQ(output.front().ticks, 100);
    }

    // Now add enough to make the buffer overflow
    buffer.push(Entry{"hello1", 100});
    buffer.push(Entry{"hello2", 200});
    buffer.push(Entry{"hello3", 300});
    buffer.push(Entry{"hello4", 400});

    {
        std::vector<Entry> output;
        ASSERT_FALSE(buffer.flush_into(output));
        // Since we overwrote the buffer we'll have weird results as promised
        ASSERT_TRUE(output.empty());
    }
}
}  // namespace miniprof
