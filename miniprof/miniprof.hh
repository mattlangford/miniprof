#include "miniprof/global.hh"
#include "miniprof/local.hh"

///
/// Global configuration options for the profiler
///
// #define ENABLE_PROFILING 1

///
/// If profiling is enabled, initialize the global profiler. This must be called in the main thread at the start of
/// execution for any profiling to work.
///
#ifdef ENABLE_PROFILING
#define maybe_init_profiler(config) ::miniprof::GlobalProfiler _global_profiler{config}
#else
#define maybe_init_profiler(config)
#endif

#define init_profiler(config) ::miniprof::GlobalProfiler _global_profiler{config}

///
/// Used for profiling a function or scope
///
#ifdef ENABLE_PROFILING
#define profile(name) ::miniprof::LocalProfiler _profiler__COUNTER__ {name}
#else
#define profile(config)
#endif
