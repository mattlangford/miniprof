#include "miniprof/local.hh"

#include <chrono>

#include "miniprof/global.hh"

namespace miniprof {

//
// #############################################################################
//

LocalProfiler::LocalProfiler(const char* name) : name_(name), start_(now()) {}

//
// #############################################################################
//

LocalProfiler::~LocalProfiler() {
    if (auto instance = GlobalProfiler::instance_ptr()) {
        auto& buffer = instance->get_buffer();
        buffer.push({name_, now() - start_});
    }
}

//
// #############################################################################
//

uint32_t LocalProfiler::now() {
    // TODO: A faster clock here
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

}  // namespace miniprof
