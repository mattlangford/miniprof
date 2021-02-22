#include "miniprof/global.hh"

#include <exception>

namespace miniprof {

//
// #############################################################################
//

GlobalProfiler* GlobalProfiler::instance_ = nullptr;

//
// #############################################################################
//

GlobalProfiler::GlobalProfiler(const Config& /*config*/) {
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

Buffer& GlobalProfiler::get_buffer() {
    thread_local ThreadRegistrar registrar;
    return *registrar.buffer;
}

//
// #############################################################################
//

std::vector<Entry> GlobalProfiler::flush() { return {}; }

//
// #############################################################################
//

GlobalProfiler& GlobalProfiler::instance() {
    if (instance_ == nullptr) {
        throw std::runtime_error("Unable to get GlobalProfiler instance. Has it been constructed?");
    }
    return *instance_;
}

//
// #############################################################################
//

GlobalProfiler::ThreadRegistrar::ThreadRegistrar() : buffer(GlobalProfiler::instance().add_buffer()) {}

//
// #############################################################################
//

GlobalProfiler::ThreadRegistrar::~ThreadRegistrar() { GlobalProfiler::instance().remove_buffer(buffer); }

//
// #############################################################################
//

std::list<Buffer>::iterator add_buffer();

//
// #############################################################################
//

void remove_buffer(std::list<Buffer>::iterator it);
}  // namespace miniprof
