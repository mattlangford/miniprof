#include <list>
#include <memory>
#include <thread>
#include <unordered_map>

#include "miniprof/buffer.hh"

namespace miniprof {

///
/// @brief Main profiler class, used to managed an arbitrary number of scoped profilers
///
class GlobalProfiler {
public:
    struct Config {
        Buffer::Config buffer_config;

        size_t flush_buffer_reserve_size = 1'000'000;
    };

public:
    explicit GlobalProfiler(const Config& config);
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

private:
    static GlobalProfiler* instance_;

    const Config config_;
    Buffer buffer_;
};
}  // namespace miniprof
