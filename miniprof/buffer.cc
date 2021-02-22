#include "miniprof/buffer.hh"

#include <iostream>

namespace miniprof {

//
// #############################################################################
//

Buffer::Buffer(const Config& config)
    : entries_(std::vector<Entry>(config.buffer_reserve_size)), access_counter_(0), write_(0), read_(0) {}

//
// #############################################################################
//

void Buffer::push(Entry entry) {
    entries_[write_] = std::move(entry);
    increment(write_);
    access_counter_++;
}

//
// #############################################################################
//

bool Buffer::flush_into(std::vector<Entry>& to) {
    while (read_ != write_) {
        // NOTE: Here is where problems arise if the read and write heads are too close and accessed concurrently
        to.push_back(entries_[read_]);
        access_counter_--;
        increment(read_);
    }

    // Since writes and reads should be 1-to-1 if this isn't zero after a read then we're in a bad state and should warn
    if (access_counter_ != 0) {
        std::cerr << "Warning! Possible loss in profiling data so the results may be unreliable. "
                  << "Increase the buffer_reserve_size for the miniprof::Buffer for more stable results\n";

        // Reset for next time
        access_counter_ = 0;
        return false;
    }
    return true;
}

//
// #############################################################################
//

void Buffer::increment(size_t& data) const { data = (data + 1) % entries_.size(); }
}  // namespace miniprof
