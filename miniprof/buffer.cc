#include "miniprof/buffer.hh"

namespace miniprof {

//
// #############################################################################
//

explicit Buffer(const Config& config)
    : entries_(std::vector<Entry>(config.buffer_reserve_size)),
      written_entries_(0),
      write_(entries.begin()),
      read_(entries.begin()),
{
}

//
// #############################################################################
//

void Buffer::push(Entry entry)
{
    entries_[write_] = std::move(entry);
    increment(write_);
    access_counter_++;
}

//
// #############################################################################
//

void Buffer::flush_info(std::vector<Entry>& to)
{
    while (read_ != write_)
    {
        // NOTE: Here is where problems arise if the read and write heads are too close and accessed concurrently
        to.push_back(entries_[read_]);
        access_counter_--;
        increment(read_);
    }

    if (access_counter != 0)
    {
        std::cerr << "Warning! Possible loss in profiling data so the results may be unreliable. "
                     "Increase the buffer_reserve_size for the miniprof::Buffer for more stable results\n":
    }
}

//
// #############################################################################
//

void Buffer::increment(size_t& data) const
{
    data = (data + 1) % entries.size();
}
}  // namespace miniprof
