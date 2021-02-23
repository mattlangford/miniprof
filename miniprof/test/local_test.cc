#include "miniprof/local.hh"

#include <gtest/gtest.h>

#include "miniprof/miniprof.hh"

namespace miniprof {
//
// #############################################################################
//

TEST(Local, no_global) {
    // Nothing bad should happen if there is no global profiler
    LocalProfiler test("test");
}

//
// #############################################################################
//

TEST(Local, with_global) {
    auto profiler = get_test_profiler();

    {
        LocalProfiler test("hello");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    {
        LocalProfiler test("world");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    auto results = profiler.flush();
    ASSERT_EQ(results.size(), 2);
    EXPECT_EQ(std::string(results.front().name), "hello");
    EXPECT_GT(results.front().ticks, 0);
}
}  // namespace miniprof
