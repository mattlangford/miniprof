#pragma once

#include <x86intrin.h>

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

// #define ENABLE_PROFILING

namespace profiler {

using Clock = std::chrono::steady_clock;

// Profiler
constexpr std::chrono::milliseconds kFlushPeriod{10};
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
    using SorterFunction =
        std::function<bool(const std::array<uint32_t, kNumCols>&, const std::array<uint32_t, kNumCols>&)>;

    template <size_t kNumCols>
    static std::string format(Buffer buffer, std::array<std::pair<std::string, MetricExtractor>, kNumCols> extractors,
                              SorterFunction<kNumCols> maybe_sort = std::nullptr_t{}) {
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
        const std::vector<std::pair<std::string, std::array<uint32_t, kNumCols>>>& row_data) {
        size_t scope_col_width = std::strlen(kScopeNameColHeader);
        std::array<size_t, kNumCols> col_widths = {};
        for (const auto& [scope_name, row] : row_data) {
            scope_col_width = std::max(scope_col_width, scope_name.size());
            for (size_t col = 0; col < kNumCols; ++col) {
                col_widths[col] = std::max({col_widths[col], std::to_string(row[col]).size(), col_headers[col].size()});
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

//
// #############################################################################
//

struct ScopedProfiler {
    ScopedProfiler(const char* name_) : name(name_), start(now()) {}
    ~ScopedProfiler() {
        constexpr size_t kScaleFactor = 1E3;
        GlobalProfiler::record(name, GlobalProfiler::Type::kTime, (now() - start) / kScaleFactor);
    }

    static size_t now() { return _rdtsc(); }

    const char* name;
    size_t start;
};
}  // namespace profiler

#ifdef ENABLE_PROFILING

#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)

#define PROFILE(name) profiler::ScopedProfiler MACRO_CONCAT(_profiler, __COUNTER__)(name);
#define PROFILE_DYN(name) \
    profiler::ScopedProfiler MACRO_CONCAT(_profiler, __COUNTER__)(profiler::GlobalProfiler::find_or_emplace(name));
#define PROFILE_DATA(name, data) profiler::GlobalProfiler::record(name, profiler::GlobalProfiler::Type::kData, data);
#define PROFILE_DATA_DYN(name, data)                                                  \
    profiler::GlobalProfiler::record(profiler::GlobalProfiler::find_or_emplace(name), \
                                     profiler::GlobalProfiler::Type::kData, data);
#else
#define PROFILE(name)
#define PROFILE_DYN(name)
#define PROFILE_DATA(name, data)
#define PROFILE_DATA_DYN(name, data)
#endif
