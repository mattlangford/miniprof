#include <benchmark/benchmark.h>

#define ENABLE_PROFILING
#include <random>

#include "miniprof/miniprof.hh"

namespace {
miniprof::GlobalProfiler::Config config() {
    miniprof::GlobalProfiler::Config config;
    config.buffer_config.buffer_reserve_size = 1000;  // small so it doesn't take long to allocate
    return config;
}
}  // namespace

static void no_profiling(benchmark::State& state) {
    size_t sleep_us = state.range(0);

    // Perform setup here
    for (auto _ : state) {
        std::this_thread::sleep_for(std::chrono::microseconds(sleep_us));
    }
}

static void with_profiling(benchmark::State& state) {
    size_t sleep_us = state.range(0);
    init_profiler(config());

    // Perform setup here
    for (auto _ : state) {
        profile("test_function");
        std::this_thread::sleep_for(std::chrono::microseconds(sleep_us));
    }
}

BENCHMARK(no_profiling)->Arg(2)->Arg(10)->Arg(100)->Arg(1000);
BENCHMARK(with_profiling)->Arg(2)->Arg(10)->Arg(100)->Arg(1000);

BENCHMARK_MAIN();
