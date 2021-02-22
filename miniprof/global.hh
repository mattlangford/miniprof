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
    struct Config {};

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
    /// invocation from a new thread.
    ///
    Buffer& get_buffer();

    ///
    /// @brief Flush all of the buffers and erase entries from unused buffers
    ///
    std::vector<Entry> flush();

    ///
    /// @brief Get access to the global profiler
    ///
    static GlobalProfiler& instance();

private:
    ///
    /// @brief RAII helper to add and remove buffers when threads are started and joined. This should be safe since at
    /// the point of a thread being joined, no profilers should be accessing the buffer
    ///
    struct ThreadRegistrar {
        ThreadRegistrar();
        ~ThreadRegistrar();

        std::list<Buffer>::iterator buffer;
    };

    ///
    /// @brief These are not thread safe and should only be called from a single thread while the given buffer isn't
    /// being used by any other threads
    ///
    std::list<Buffer>::iterator add_buffer();
    void remove_buffer(std::list<Buffer>::iterator it);

private:
    static GlobalProfiler* instance_;

    std::mutex buffers_mutex_;
    std::list<Buffer> buffers_;
};
}  // namespace miniprof
