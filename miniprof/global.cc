#include "miniprof/global.hh"

#include <exception>
#include <filesystem>
#include <iostream>

namespace miniprof {

//
// #############################################################################
//

GlobalProfiler* GlobalProfiler::instance_ = nullptr;

//
// #############################################################################
//

GlobalProfiler::GlobalProfiler(const Config& config, std::unique_ptr<OutputBase> output)
    : config_(config), buffer_(config_.buffer_config), shutdown_(false), output_(std::move(output)) {
    if (instance_ != nullptr) {
        throw std::runtime_error("Trying to create multiple global profilers. This is not allowed!");
    }
    instance_ = this;
    flush_thread_ = std::thread{[this]() { flush_thread_loop(); }};
}

//
// #############################################################################
//

GlobalProfiler::~GlobalProfiler() { shutdown(); }

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

OutputBase& GlobalProfiler::get_output() { return *output_; }

//
// #############################################################################
//

void GlobalProfiler::shutdown() {
    instance_ = nullptr;

    shutdown_ = true;
    if (flush_thread_.joinable()) flush_thread_.join();
}

//
// #############################################################################
//

void GlobalProfiler::flush_thread_loop() {
    // No flushing with non-positive interval
    if (config_.flush_interval.count() <= 0) return;

    while (!shutdown_) {
        std::this_thread::sleep_for(config_.flush_interval);

        output_->handle_entries(flush());
    }

    output_->handle_entries(flush());
}
}  // namespace miniprof
