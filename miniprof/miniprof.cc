#include "miniprof/miniprof.hh"

#include "miniprof/output.hh"

namespace miniprof {

//
// #############################################################################
//

GlobalProfiler::Config get_default_config() {
    GlobalProfiler::Config config;
    config.buffer_config.buffer_size = 100'000;
    config.flush_buffer_reserve_size = 10'000;
    config.flush_interval = std::chrono::milliseconds(100);
    config.output = "/tmp/prof.csv";
    return config;
}

//
// #############################################################################
//

GlobalProfiler get_default_profiler(const GlobalProfiler::Config& config) { return get_csv_writing_profiler(config); }

//
// #############################################################################
//

GlobalProfiler::Config get_test_config() {
    GlobalProfiler::Config config;
    config.buffer_config.buffer_size = 5000;               // small so it doesn't take long to allocate
    config.flush_interval = std::chrono::milliseconds(0);  // no flushing
    return config;
}

//
// #############################################################################
//

GlobalProfiler get_test_profiler(const GlobalProfiler::Config& config) {
    return GlobalProfiler{config, std::make_unique<TestOutput>()};
}

//
// #############################################################################
//

GlobalProfiler get_csv_writing_profiler(const GlobalProfiler::Config& config, const std::filesystem::path& output) {
    return GlobalProfiler{config, std::make_unique<CSVOutput>(output)};
}
}  // namespace miniprof
