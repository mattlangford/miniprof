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

GlobalProfiler::GlobalProfiler(const Config& config)
    : config_(config), buffer_(config_.buffer_config), shutdown_(false) {
    if (instance_ != nullptr) {
        throw std::runtime_error("Trying to create multiple global profilers. This is not allowed!");
    }
    instance_ = this;
    flush_thread_ = std::thread{[this]() { flush_thread_loop(); }};
}

//
// #############################################################################
//

GlobalProfiler::~GlobalProfiler() {
    instance_ = nullptr;

    shutdown_ = true;
    if (flush_thread_.joinable()) flush_thread_.join();
}

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

//
// #############################################################################
//

void GlobalProfiler::flush_thread_loop() {
    // No flushing with non-positive interval
    if (config_.flush_interval.count() <= 0) return;

    size_t total_flushed = 0;
    while (!shutdown_) {
        std::this_thread::sleep_for(config_.flush_interval);
        total_flushed += flush().size();
    }

    total_flushed += flush().size();
    //std::cout << "Total Flushed Entries: " << total_flushed << "\n";
}
}  // namespace miniprof
