#include "miniprof/buffer.hh"

#include <gtest/gtest.h>

namespace miniprof {

TEST(Buffer, basic) {
    Buffer::Config config;
    config.buffer_reserve_size = 4;

    Buffer buffer{config};
    buffer.push(Entry{100});
    buffer.push(Entry{200});
    buffer.push(Entry{300});

    // The buffer should be at maximum capacity now

    {
        std::vector<Entry> output;
        buffer.flush_into(output);
        ASSERT_EQ(output.size(), 3);
        EXPECT_EQ(output.front().ticks, 100);
    }

    // Now add enough to make the buffer overflow
    buffer.push(Entry{100});
    buffer.push(Entry{200});
    buffer.push(Entry{300});
    buffer.push(Entry{400});

    {
        std::vector<Entry> output;
        buffer.flush_into(output);
        // Since we overwrote the buffer we'll have weird results as promised
        ASSERT_TRUE(output.empty());
    }
}
}  // namespace miniprof
