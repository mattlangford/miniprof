#include "miniprof/local.hh"

#include <gtest/gtest.h>

#include "miniprof/global.hh"

namespace miniprof {
namespace {
GlobalProfiler::Config get_config() {
    GlobalProfiler::Config config;
    config.buffer_config.buffer_size = 1000;  // small so it doesn't take long to allocate
    return config;
}

GlobalProfiler get_profiler(const GlobalProfiler::Config& config = get_config()) {
    return GlobalProfiler{config, std::make_unique<TestOutput>()};
}
}  // namespace

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
    auto profiler = get_profiler();

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
