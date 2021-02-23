#include <benchmark/benchmark.h>

#include <iostream>
#include <random>

#define ENABLE_PROFILING
#include "miniprof/miniprof.hh"

namespace {
miniprof::GlobalProfiler::Config config() {
    miniprof::GlobalProfiler::Config config;
    config.buffer_config.buffer_size = 100000;
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
    miniprof::GlobalProfiler{config(), std::make_unique<miniprof::CSVOutput>("/tmp/prof")};

    // Perform setup here
    for (auto _ : state) {
        profile("test_function");
        std::this_thread::sleep_for(std::chrono::microseconds(sleep_us));
    }
}

BENCHMARK(no_profiling)->Arg(2)->Arg(10)->Arg(100)->Arg(1000);
BENCHMARK(with_profiling)->Arg(2)->Arg(10)->Arg(100)->Arg(1000);

BENCHMARK_MAIN();
