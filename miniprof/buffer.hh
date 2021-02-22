namespace miniprof {

struct Entry {
    uint32_t ticks;
};

class Buffer {
public:
    struct Config {
        size_t buffer_size;
    };

public:
    Buffer(const Config& config);

    ~Buffer() = default;
    Buffer(const Buffer& rhs) = delete;
    Buffer(Buffer&& rhs) = delete;
    Buffer& operator=(const Buffer& rhs) = delete;
    Buffer& operator=(Buffer&& rhs) = delete;

public:
    void push(Entry entry);
    size_t flush_info(std::vector<Entry>& entries);

private:
};
}  // namespace miniprof
