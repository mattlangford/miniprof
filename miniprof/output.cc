#include "miniprof/output.hh"

#include <iostream>

#include "miniprof/buffer.hh"

namespace miniprof {

//
// #############################################################################
//

CSVOutput::CSVOutput(const std::filesystem::path& path) {
    std::filesystem::create_directories(path.parent_path());
    stream_.open(path);
}

//
// #############################################################################
//

CSVOutput::~CSVOutput() { stream_.close(); }

//
// #############################################################################
//

void CSVOutput::handle_entries(const std::vector<Entry>& entries) {
    for (const Entry& entry : entries) stream_ << entry.name << ',' << entry.ticks << "\n";
}

//
// #############################################################################
//

void TestOutput::handle_entries(const std::vector<Entry>& entries) {
    for (const auto& entry : entries) entries_.emplace_back(entry);
}

//
// #############################################################################
//

const std::vector<Entry>& TestOutput::get_entries() const { return entries_; }
}  // namespace miniprof
