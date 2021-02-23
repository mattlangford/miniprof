#pragma once
#include "miniprof/global.hh"
#include "miniprof/local.hh"

///
/// Global configuration options for the profiler
///
// #define ENABLE_PROFILING 1

///
/// Used for profiling a function or scope
///
#ifdef ENABLE_PROFILING
#define profile(name) \
    ::miniprof::LocalProfiler _profiler__COUNTER__ { name }
#else
#define profile(config)
#endif
