#include <vector>
#include <cstddef>

namespace miniprof {

struct Entry {
    uint32_t ticks;
};

///
/// @brief Simple circular buffer for single producer single consumer type access where writes and reads can happen
/// concurrently as long as the buffer isn't close to capacity.
///
class Buffer {
public:
    struct Config {
        size_t buffer_reserve_size;
    };

public:
    explicit Buffer(const Config& config)
        : entries_(std::vector<Entry>(config.buffer_reserve_size)),
          written_entries_(0),
          write_(entries.begin()),
          read_(entries.begin()),
    {
    }

    ~Buffer() = default;
    Buffer(const Buffer& rhs) = delete;
    Buffer(Buffer&& rhs) = delete;
    Buffer& operator=(const Buffer& rhs) = delete;
    Buffer& operator=(Buffer&& rhs) = delete;

public:
    ///
    /// @brief Add new entries to the buffer or remove all of the current entries.
    /// NOTE: These are okay to call concurrently provided the buffer has plenty of extra space.
    ///
    void push(Entry entry);
    void flush_info(std::vector<Entry>& to);

private:
    void increment(size_t& data) const;

private:
    /// Main data store, this vector isn't resized after construction so it's safe to keep pointers
    std::vector<Entry> entries_;
    /// Incremented whenever an entry is added and decremented when it's removed. Used to keep track
    /// of when we've dropped data
    int access_counter_;

    /// Write and read heads for accessing data
    size_t write_;
    size_t read_;
};
}  // namespace miniprof
