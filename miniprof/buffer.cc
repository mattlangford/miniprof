#include "miniprof/buffer.hh"

#include <iostream>

namespace miniprof {

//
// #############################################################################
//

Buffer::Buffer(const Config& config) : entries_(std::vector<Entry>(config.buffer_size)) {}

//
// #############################################################################
//

void Buffer::push(const Entry& entry) { entries_[(write_++) % entries_.size()] = entry; }

//
// #############################################################################
//

bool Buffer::flush_into(std::vector<Entry>& to) {
    // Get the current write state. Note that writes may happen after we load this, but we'll ignore them for this flush
    uint64_t current_write = write_.load();
    uint64_t current_write_index = current_write % entries_.size();
    uint64_t current_read_index = read_ % entries_.size();

    while (current_read_index != current_write_index) {
        // NOTE: Here is where problems arise if the read and write heads are too close and accessed concurrently
        to.push_back(entries_[current_read_index]);
        current_read_index = ++read_ % entries_.size();
    }

    // Since writes and reads should be 1-to-1 if this isn't zero after a read then we're in a bad state and should warn
    if (read_ != current_write) {
        std::cerr << "Warning! Possible loss in profiling data so the results may be unreliable. "
                  << "Increase the buffer_size for the miniprof::Buffer for more stable results\n";
        return false;
    }
    return true;
}
}  // namespace miniprof
