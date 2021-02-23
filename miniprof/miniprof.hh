#pragma once
#include "miniprof/global.hh"
#include "miniprof/local.hh"

///
/// Global configuration options for the profiler
///
// #define ENABLE_PROFILING 1

///
/// For initializing the global profiler
///
#ifdef ENABLE_PROFILING
#define init_deafult_profiler() ::miniprof::GlobalProfiler _profiler = get_default_profiler();
#else
#define init_deafult_profiler()
#endif

///
/// Used for profiling a function or scope
///
#ifdef ENABLE_PROFILING
#define profile(name) \
    ::miniprof::LocalProfiler _profiler__COUNTER__ { name }
#else
#define profile(name)
#endif

namespace miniprof {

///
/// @brief Generate a "good" default profiler, this will have default configs
///
GlobalProfiler::Config get_default_config();
GlobalProfiler get_default_profiler(const GlobalProfiler::Config& config = get_default_config());

///
/// @brief Generate a test profiler for use in unit tests
///
GlobalProfiler::Config get_test_config();
GlobalProfiler get_test_profiler(const GlobalProfiler::Config& config = get_test_config());

///
/// @brief Generate a profiler that dumps data to the CSV file
///
GlobalProfiler get_csv_writing_profiler(const GlobalProfiler::Config& config = {},
                                        const std::filesystem::path& output = "/tmp/prof");
}  // namespace miniprof
