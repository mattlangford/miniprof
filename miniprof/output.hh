#pragma once
#include <filesystem>
#include <fstream>
#include <variant>
#include <vector>

namespace miniprof {

struct Entry;

class OutputBase {
public:
    virtual ~OutputBase() = default;
    virtual void handle_entries(const std::vector<Entry>& entries) = 0;
};

//
// #############################################################################
//

class CSVOutput : public OutputBase {
public:
    CSVOutput(const std::filesystem::path& path);
    ~CSVOutput() override;
    void handle_entries(const std::vector<Entry>& entries) override;

private:
    std::ofstream stream_;
};

//
// #############################################################################
//

class TestOutput : public OutputBase {
public:
    TestOutput() = default;
    ~TestOutput() override = default;
    void handle_entries(const std::vector<Entry>& entries) override;
    const std::vector<Entry>& get_entries() const;

private:
    std::vector<Entry> entries_;
};
}  // namespace miniprof
