#pragma once
#include <list>
#include <memory>
#include <thread>
#include <unordered_map>

#include "miniprof/buffer.hh"
#include "miniprof/output.hh"

namespace miniprof {

///
/// @brief Main profiler class, used to managed an arbitrary number of scoped profilers
///
class GlobalProfiler {
public:
    struct Config {
        /// Configuration for the buffer
        Buffer::Config buffer_config;

        /// How much space to reserve per call to flush
        size_t flush_buffer_reserve_size = 1'000'000;

        /// No flushing if <= 0
        std::chrono::milliseconds flush_interval = std::chrono::milliseconds(10);

        /// Where to dump data to
        std::string output = "/tmp/prof.csv";
    };

public:
    GlobalProfiler(const Config& config, std::unique_ptr<OutputBase> output);
    ~GlobalProfiler();

    GlobalProfiler(const GlobalProfiler& rhs) = delete;
    GlobalProfiler(GlobalProfiler&& rhs) = delete;
    GlobalProfiler& operator=(const GlobalProfiler& rhs) = delete;
    GlobalProfiler& operator=(GlobalProfiler&& rhs) = delete;

public:
    ///
    /// @brief Get the appropriate buffer for the given thread. This is generally pretty quick except for the first
    /// invocation from a new thread. This does not acquire any locks.
    ///
    Buffer& get_buffer();

    ///
    /// @brief Flush all of the buffers and erase entries from unused buffers. This may acquire the lock since it may
    /// also remove unused buffers.
    ///
    std::vector<Entry> flush();

    ///
    /// @brief Get access to the global profiler. The reference version will throw if no instance is set
    ///
    static GlobalProfiler& instance();
    static GlobalProfiler* instance_ptr();

    ///
    /// @brief Get the current output module
    ///
    OutputBase& get_output();

    ///
    /// @brief Force a shutdown of the profiler, flushing all output and clearing the global instance
    ///
    void shutdown();

private:
    void flush_thread_loop();

private:
    static GlobalProfiler* instance_;

    const Config config_;
    Buffer buffer_;

    std::atomic_bool shutdown_;
    std::thread flush_thread_;

    std::unique_ptr<OutputBase> output_;
};
}  // namespace miniprof
