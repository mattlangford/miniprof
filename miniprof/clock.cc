#include "miniprof/clock.hh"

namespace miniprof {
uint32_t now() {
    // TODO: A faster clock here
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch())
        .count();
}
}  // namespace miniprof
