#include <cstdint>

namespace miniprof {

class LocalProfiler {
public:
    explicit LocalProfiler(const char* name);
    ~LocalProfiler();

    LocalProfiler(const LocalProfiler& rhs) = delete;
    LocalProfiler(LocalProfiler&& rhs) = delete;
    LocalProfiler& operator=(const LocalProfiler& rhs) = delete;
    LocalProfiler& operator=(LocalProfiler&& rhs) = delete;

private:
    static uint32_t now();

    const char* name_;
    uint32_t start_;
};
}  // namespace miniprof
