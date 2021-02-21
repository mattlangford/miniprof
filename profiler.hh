#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <x86intrin.h>

// #define ENABLE_PROFILING

namespace profiler {

using Clock = std::chrono::steady_clock;

// Profiler
constexpr std::chrono::milliseconds kFlushPeriod { 10 };
constexpr size_t kReserveSize = 500'000'000;
static constexpr size_t kWriteSwapSize = 0.1 * kReserveSize;

// Output Formatting
static constexpr size_t kMetricWidth = 5;
static constexpr auto kScopeNameColHeader = "Scope Name";

//
// #############################################################################
//

class Formatter {
public:
    using MetricExtractor = std::function<uint32_t(const std::vector<uint32_t>&)>;
    using Buffer = std::vector<std::pair<const char*, uint32_t>>;

    template <size_t kNumCols>
    using SorterFunction = std::function<bool(const std::array<uint32_t, kNumCols>&, const std::array<uint32_t, kNumCols>&)>;

    template <size_t kNumCols>
    static std::string format(
        Buffer buffer,
        std::array<std::pair<std::string, MetricExtractor>, kNumCols> extractors,
        SorterFunction<kNumCols> maybe_sort = std::nullptr_t {})
    {
        if (buffer.empty()) {
            return "No profiling data captured!";
        }
        std::cout << "Processing " << buffer.size() << " entries..." << std::endl;

        std::unordered_map<std::string, std::vector<uint32_t>> raw_data;
        for (const auto& [key, data] : buffer) {
            raw_data[key].push_back(data);
        }

        std::array<std::string, kNumCols> col_headers;
        std::vector<std::pair<std::string, std::array<uint32_t, kNumCols>>> row_data;
        for (auto& [key, data] : raw_data) {
            std::sort(data.begin(), data.end());
            auto& row = row_data.emplace_back();
            row.first = key;
            for (size_t col = 0; col < kNumCols; ++col) {
                // Save the header
                col_headers[col] = extractors[col].first;

                // Save the row data
                row.second[col] = extractors[col].second(data);
            }
        }

        std::sort(row_data.begin(), row_data.end(), [&](const auto& lhs, const auto& rhs) {
            return maybe_sort ? maybe_sort(lhs.second, rhs.second) : lhs.first < rhs.first;
        });

        return format_table(col_headers, row_data);
    }

private:
    template <size_t kNumCols>
    static std::string format_table(
        const std::array<std::string, kNumCols> col_headers,
        const std::vector<std::pair<std::string, std::array<uint32_t, kNumCols>>>& row_data)
    {
        size_t scope_col_width = std::strlen(kScopeNameColHeader);
        std::array<size_t, kNumCols> col_widths = {};
        for (const auto& [scope_name, row] : row_data) {
            scope_col_width = std::max(scope_col_width, scope_name.size());
            for (size_t col = 0; col < kNumCols; ++col) {
                col_widths[col] = std::max({ col_widths[col], std::to_string(row[col]).size(), col_headers[col].size() });
            }
        }

        std::stringstream output;
        constexpr auto kSpacer = " | ";

        // Print out headers
        output << kSpacer << std::setw(scope_col_width) << kScopeNameColHeader << kSpacer;
        for (size_t col = 0; col < kNumCols; ++col) {
            output << std::setw(col_widths[col]) << col_headers[col] << kSpacer;
        }
        output << "\n";

        // Serialize each row (since this is a map it'll also sort them)
        for (const auto& [scope_name, row] : row_data) {
            output << kSpacer << std::setw(scope_col_width) << scope_name << kSpacer;
            for (size_t col = 0; col < kNumCols; ++col) {
                output << std::setw(col_widths[col]) << row[col] << kSpacer;
            }
            output << "\n";
        }

        return output.str();
    }
};

//
// #############################################################################
//

class GlobalProfiler {
public:
    enum Type : uint8_t {
        kTime = 0,
        kData = 1,
        kNumTypes = 2
    };

private:
    struct SpinLock {
        SpinLock() { unlock(); }
        bool maybe_lock() { return std::atomic_exchange_explicit(&lock_, true, std::memory_order_acquire); }
        void lock()
        {
            while (maybe_lock())
                ;
        }
        void unlock() { std::atomic_store_explicit(&lock_, false, std::memory_order_release); }

        // holds true when write and read shouldn't be swapped (since one is being written to/read from)
        std::atomic<bool> lock_;
    };

    using EntriesMap = std::vector<std::pair<const char*, uint32_t>>;
    using Buffer = std::array<EntriesMap, Type::kNumTypes>;

    struct Storage {
        Buffer write;
        Buffer read;

        SpinLock swappable;
        std::atomic<bool> read_empty;
    };

    inline static Storage* storage;

    // This is all to handle dynamic strings
    static constexpr size_t kDynamicSize = 255;
    inline static std::deque<std::array<char, kDynamicSize + 1>> dynamic_string_storage;

public:
    static const char* find_or_emplace(std::string new_str)
    {
        if (new_str.size() < kDynamicSize) {
            new_str.resize(kDynamicSize);
        }

        const char* new_str_ptr = new_str.c_str();
        for (const auto& arr : dynamic_string_storage) {
            if (std::strcmp(arr.data(), new_str_ptr) == 0) {
                return arr.data();
            }
        }
        auto& arr = dynamic_string_storage.emplace_back();
        std::strcpy(arr.data(), new_str_ptr);
        return arr.data();
    }

private:
    void flush()
    {
        // Spin lock until the buffer is unswappable (since we'll be reading from the read buffer)
        storage_.swappable.lock();

        for (size_t index = 0; index < Type::kNumTypes; ++index) {
            auto& from_data = storage_.read[index];
            auto& to_data = backend_[index];

            std::copy(from_data.begin(), from_data.end(), std::back_inserter(to_data));
            from_data.clear();
        }
        storage_.read_empty = true;
        storage_.swappable.unlock();
    }

    void shutdown()
    {
        shutdown_ = true;
        if (flush_thread_.joinable()) {
            flush_thread_.join();
        }
    }

public:
    static void record(const char* key, Type type, uint32_t data)
    {
        auto& data_record = storage->write[type];
        data_record.push_back({ key, data });

        // Should we try to do a swap?
        if (data_record.size() >= kReserveSize) {
            std::cerr << "Dropping oldest " << kWriteSwapSize
                      << " samples! Adjust the kFlushPeriod, kReserveSize, or kWriteSwapSize\n";
            data_record.erase(data_record.begin(), data_record.begin() + kWriteSwapSize);
        }

        if (data_record.size() >= kWriteSwapSize) {
            // If the other thread hasn't cleared the read buffer we can continue
            if (!storage->read_empty) {
                return;
            }

            if (storage->swappable.maybe_lock()) {
                std::swap(storage->write, storage->read);
                storage->read_empty = false;
                storage->swappable.unlock();
            }
        }
    }

public:
    GlobalProfiler()
    {
#ifndef ENABLE_PROFILING
        return;
#endif
        storage = &storage_;
        for (auto& write : storage_.write)
            write.reserve(kReserveSize);

        flush_thread_ = std::thread([this]() {
            while (!shutdown_) {
                flush();
                std::this_thread::sleep_for(kFlushPeriod);
            };
        });
    }

    ~GlobalProfiler()
    {
        shutdown();
    }
    std::string generate_stats_table()
    {
#ifndef ENABLE_PROFILING
        return "Profiling disabled";
#endif
        shutdown();

        // Flush the current storage_.read
        flush();
        // Then swap and flush the storage_.write
        std::swap(storage_.write, storage_.read);
        flush();

        std::string output;

        // First we'll print out timing data
        // clang-format off
        output += Formatter::format<4>(backend_[Type::kTime], {
            std::pair { "Count", [](const std::vector<uint32_t>& d) -> uint32_t {
                return d.size();
            }},
            std::pair { "P50", [](const std::vector<uint32_t>& d) -> uint32_t {
                return d.empty() ? 0 : d[0.50 * d.size()];
            }},
            std::pair { "P99", [](const std::vector<uint32_t>& d) -> uint32_t {
                return d.empty() ? 0 : d[0.99 * d.size()];
            }},
            std::pair { "Total", [](const std::vector<uint32_t>& d) -> uint32_t {
                return d.empty() ? 0 : std::accumulate(d.begin(), d.end(), 0);
            }}
        }, [](const auto& lhs, const auto& rhs){ return lhs[3] < rhs[3]; });
        // clang-format on

        output += "\n";

        // Then the data values
        // clang-format off
        output += Formatter::format<2>(backend_[Type::kData], {
            std::pair { "Count", [](const std::vector<uint32_t>& d) -> uint32_t {
                return d.size();
            }},
            std::pair { "P50", [](const std::vector<uint32_t>& d) -> uint32_t {
                return d.empty() ? 0 : d[0.50 * d.size()];
            }},
        }, [](const auto& lhs, const auto& rhs){ return lhs[1] < rhs[1]; });
        // clang-format on

        return output;
    }

private:
    std::atomic<bool> shutdown_ = false;
    std::thread flush_thread_;
    Buffer backend_;
    Storage storage_;
};

//
// #############################################################################
//

struct ScopedProfiler {
    ScopedProfiler(const char* name_)
        : name(name_)
        , start(now())
    {
    }
    ~ScopedProfiler()
    {
        constexpr size_t kScaleFactor = 1E3;
        GlobalProfiler::record(name, GlobalProfiler::Type::kTime, (now() - start) / kScaleFactor);
    }

    static size_t now() { return _rdtsc(); }

    const char* name;
    size_t start;
};
}

#ifdef ENABLE_PROFILING

#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)

#define PROFILE(name) profiler::ScopedProfiler MACRO_CONCAT(_profiler, __COUNTER__)(name);
#define PROFILE_DYN(name) profiler::ScopedProfiler MACRO_CONCAT(_profiler, __COUNTER__)(profiler::GlobalProfiler::find_or_emplace(name));
#define PROFILE_DATA(name, data) profiler::GlobalProfiler::record(name, profiler::GlobalProfiler::Type::kData, data);
#define PROFILE_DATA_DYN(name, data) profiler::GlobalProfiler::record(profiler::GlobalProfiler::find_or_emplace(name), profiler::GlobalProfiler::Type::kData, data);
#else
#define PROFILE(name)
#define PROFILE_DYN(name)
#define PROFILE_DATA(name, data)
#define PROFILE_DATA_DYN(name, data)
#endif
