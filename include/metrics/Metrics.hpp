#pragma once

#include <atomic>
#include <cstdint>

class Metrics {
public:
    void record_submitted();
    void record_completed();
    void print_summary() const;

private:
    std::atomic<uint64_t> submitted_{0};
    std::atomic<uint64_t> completed_{0};
};
