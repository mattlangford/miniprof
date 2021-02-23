#pragma once
#include "miniprof/global.hh"
#include "miniprof/local.hh"

///
/// Global configuration options for the profiler
///
// #define ENABLE_PROFILING 1

#define REQ_SEMICOLON \
    do {              \
    } while (false)

///
/// For initializing the global profiler
///
#ifdef ENABLE_PROFILING
#define init_default_profiler() ::miniprof::GlobalProfiler _profiler = ::miniprof::get_default_profiler()
#else
#define init_default_profiler() REQ_SEMICOLON
#endif

///
/// Used for profiling a function or scope
///
#ifdef ENABLE_PROFILING
#define profile(name) \
    ::miniprof::LocalProfiler _profiler__COUNTER__ { name }
#define profile_function() \
    ::miniprof::LocalProfiler _profiler__COUNTER__ { __FUNCTION__ }
#else
#define profile(name) REQ_SEMICOLON
#define profile_function() REQ_SEMICOLON
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
