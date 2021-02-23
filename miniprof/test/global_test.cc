#include "miniprof/global.hh"

#include <gtest/gtest.h>

#include "miniprof/miniprof.hh"
#include "miniprof/output.hh"

namespace miniprof {
TEST(Global, construct) {
    {
        auto profiler = get_test_profiler();
        EXPECT_EQ(&GlobalProfiler::instance(), &profiler);

        // Already constructed!
        EXPECT_THROW(get_test_profiler(), std::runtime_error);
    }

    auto profiler = get_test_profiler();
    EXPECT_EQ(&GlobalProfiler::instance(), &profiler);
}

//
// #############################################################################
//

TEST(Global, write_to_buffer) {
    auto profiler = get_test_profiler();
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
    auto profiler = get_test_profiler();

    constexpr size_t kNumThreads = 30;
    constexpr size_t kNumWritesPerThread = 100;

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

//
// #############################################################################
//

TEST(Global, flush_thread) {
    auto config = get_test_config();
    config.flush_interval = std::chrono::milliseconds(10);
    {
        auto profiler = get_test_profiler(config);

        auto& buffer = profiler.get_buffer();
        buffer.push({"test1", 100});
        buffer.push({"test2", 200});
    }
}

//
// #############################################################################
//

TEST(Global, file_output) {
    {
        auto profiler = get_csv_writing_profiler();

        auto& buffer = profiler.get_buffer();
        buffer.push({"test1", 100});
        buffer.push({"test2", 200});
    }
}
}  // namespace miniprof
