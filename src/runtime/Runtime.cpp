#include "runtime/Runtime.hpp"

Runtime::Runtime(int num_workers) {
    for (int i = 0; i < num_workers; ++i) {
        // Worker needs: its id, the shared queue, a shutdown flag, and a
        // callback to fire when it completes a task.
        // Worker never includes Runtime.hpp — this lambda is the only link back.
        workers_.push_back(std::make_unique<Worker>(
            i,
            queue_,
            shutdown_flag_,
            [this]() { on_task_complete(); }
        ));
    }

    for (auto& w : workers_) {
        w->start();
    }
}

Runtime::~Runtime() {
    shutdown();
}

void Runtime::submit(std::function<void()> fn) {
    Task t;
    t.task_id = next_id_++;
    t.fn      = std::move(fn);

    // increment submitted BEFORE pushing so completed can never overtake it
    submitted_.fetch_add(1, std::memory_order_relaxed);
    metrics_.record_submitted();

    queue_.push(std::move(t));
}

void Runtime::wait_all() {
    std::unique_lock<std::mutex> lock(cv_mutex_);
    cv_.wait(lock, [this]() {
        return submitted_.load(std::memory_order_relaxed) > 0 &&
               completed_.load(std::memory_order_relaxed) >=
               submitted_.load(std::memory_order_relaxed);
    });
}

void Runtime::on_task_complete() {
    metrics_.record_completed();
    uint64_t done = completed_.fetch_add(1, std::memory_order_relaxed) + 1;

    if (done >= submitted_.load(std::memory_order_relaxed)) {
        cv_.notify_all();
    }
}

void Runtime::shutdown() {
    shutdown_flag_.store(true, std::memory_order_relaxed);
    for (auto& w : workers_) {
        w->join();
    }
}
