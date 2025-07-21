#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <vector>

#include "core/snapshot.h"

class SystemRingBuffer {
public:
    explicit SystemRingBuffer(size_t capacity, size_t retain_count);

    bool pushHead(SystemSnapshotPtr snapshot);
    bool pushTail(SystemSnapshotPtr snapshot);

    std::optional<double> headTime() const;
    std::optional<double> tailTime() const;

    std::optional<SystemSnapshotPtr> readForward();
    std::optional<SystemSnapshotPtr> readBackward();

    void clear();
    void shutdown();

    bool isFull() const;
    bool isEmpty() const;
    bool hasReaderCaughtUp() const;
    size_t size() const;

private:
    size_t advance(size_t idx) const;
    size_t retreat(size_t idx) const;
    size_t distance(size_t from, size_t to) const;

    std::vector<SystemSnapshotPtr> buffer_;

    const size_t retain_count_;  // How many snapshots behind read_ we must retain

    size_t tail_ = 0;  // Oldest retained
    size_t read_ = 0;  // Current read position
    size_t head_ = 0;  // Right after the last written

    mutable std::mutex mtx_;
    std::condition_variable cv_;

    std::atomic<bool> is_shutting_down_ = false;
};
