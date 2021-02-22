#include "miniprof/global.hh"

#include <algorithm>
#include <exception>
#include <iostream>

namespace miniprof {

//
// #############################################################################
//

GlobalProfiler* GlobalProfiler::instance_ = nullptr;

//
// #############################################################################
//

GlobalProfiler::GlobalProfiler(const Config& config) : config_(config), buffer_(config_.buffer_config) {
    if (instance_ != nullptr) {
        throw std::runtime_error("Trying to create multiple global profilers. This is not allowed!");
    }
    instance_ = this;
}

//
// #############################################################################
//

GlobalProfiler::~GlobalProfiler() { instance_ = nullptr; }

//
// #############################################################################
//

Buffer& GlobalProfiler::get_buffer() { return buffer_; }

//
// #############################################################################
//

std::vector<Entry> GlobalProfiler::flush() {
    std::vector<Entry> entries;
    entries.reserve(config_.flush_buffer_reserve_size);

    if (!buffer_.flush_into(entries)) {
        throw std::runtime_error("Error reading from buffer.");
    }

    return entries;
}

//
// #############################################################################
//

GlobalProfiler& GlobalProfiler::instance() {
    auto ptr = instance_ptr();
    if (ptr == nullptr) {
        throw std::runtime_error("Unable to get GlobalProfiler instance. Has it been constructed?");
    }
    return *ptr;
}

//
// #############################################################################
//

GlobalProfiler* GlobalProfiler::instance_ptr() { return instance_; }
}  // namespace miniprof
