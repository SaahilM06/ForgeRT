#include "metrics/Metrics.hpp"
#include <cinttypes>
#include <cstdio>

void Metrics::record_submitted() {
    submitted_.fetch_add(1, std::memory_order_relaxed);
}

void Metrics::record_completed() {
    completed_.fetch_add(1, std::memory_order_relaxed);
}

void Metrics::print_summary() const {
    std::printf("[Metrics] submitted=%" PRIu64 "  completed=%" PRIu64 "\n",
                submitted_.load(std::memory_order_relaxed),
                completed_.load(std::memory_order_relaxed));
}
