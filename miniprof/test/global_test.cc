#include "miniprof/global.hh"

#include <gtest/gtest.h>

namespace miniprof {

TEST(Global, construct) {
    GlobalProfiler::Config config;

    {
        GlobalProfiler profiler{config};
        EXPECT_EQ(&GlobalProfiler::instance(), &profiler);

        // Already constructed!
        EXPECT_THROW(GlobalProfiler{config}, std::runtime_error);
    }

    GlobalProfiler profiler{config};
    EXPECT_EQ(&GlobalProfiler::instance(), &profiler);
}
}  // namespace miniprof
