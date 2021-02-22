#include "miniprof/local.hh"

#include <chrono>

#include "miniprof/clock.hh"
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

}  // namespace miniprof
