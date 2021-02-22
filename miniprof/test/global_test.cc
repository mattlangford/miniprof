#include "miniprof/global.hh"

#include <gtest/gtest.h>

namespace miniprof {
namespace {
GlobalProfiler::Config config() {
    GlobalProfiler::Config config;
    config.buffer_config.buffer_reserve_size = 1000;  // small so it doesn't take long to allocate
    return config;
}
}  // namespace

TEST(Global, construct) {
    {
        GlobalProfiler profiler{config()};
        EXPECT_EQ(&GlobalProfiler::instance(), &profiler);

        // Already constructed!
        EXPECT_THROW(GlobalProfiler{config()}, std::runtime_error);
    }

    GlobalProfiler profiler{config()};
    EXPECT_EQ(&GlobalProfiler::instance(), &profiler);
}

//
// #############################################################################
//

TEST(Global, write_to_buffer) {
    GlobalProfiler profiler{config()};
    auto& buffer = profiler.get_buffer();
    buffer.push({"test1", 100});
    buffer.push({"test2", 200});

    auto results = profiler.flush();
    ASSERT_EQ(results.size(), 2);
    EXPECT_EQ(results.front().name, "test1");
    EXPECT_EQ(results.front().ticks, 100);
}

//
// #############################################################################
//

TEST(Global, write_to_buffer_thread) {
    GlobalProfiler profiler{config()};

    constexpr size_t kNumThreads = 30;
    constexpr size_t kNumWritesPerThread = 10;

    std::vector<std::thread> threads;
    for (size_t i = 0; i < kNumThreads; ++i) {
        threads.emplace_back([]() {
            auto& buffer = GlobalProfiler::instance().get_buffer();
            for (size_t i = 0; i < kNumWritesPerThread; ++i) {
                buffer.push({"entry", static_cast<uint32_t>(i)});
            }
        });
    }
    for (auto& thread : threads) thread.join();

    auto results = profiler.flush();
    ASSERT_EQ(results.size(), kNumThreads * kNumWritesPerThread);
    for (size_t i = 0; i < results.size(); ++i) {
        EXPECT_EQ(std::string(results.front().name), "entry");

        // Should be something between 0 and kNumWritesPerThread
        EXPECT_GE(results.front().ticks, 0);
        EXPECT_LT(results.front().ticks, kNumWritesPerThread);
    }
}
}  // namespace miniprof
