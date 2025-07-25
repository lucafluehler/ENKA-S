#include "core/dataflow/system_ring_buffer.h"

#include <condition_variable>
#include <mutex>
#include <optional>
#include <vector>

SystemRingBuffer::SystemRingBuffer(size_t capacity, size_t retain_count)
    : retain_count_(retain_count), buffer_(capacity + 1) {}

bool SystemRingBuffer::pushHead(SystemSnapshotPtr snapshot) {
    std::unique_lock lock(mtx_);

    // We cannot push if the buffer is full, so we wait until space is available
    cv_.wait(lock, [&] { return !isFull() || is_shutting_down_.load(); });

    if (is_shutting_down_.load()) return false;

    buffer_[head_] = std::move(snapshot);
    head_ = advance(head_);

    cv_.notify_all();
    return true;
}

bool SystemRingBuffer::pushTail(SystemSnapshotPtr snapshot) {
    std::unique_lock lock(mtx_);

    // If the buffer is full, we must also retreat the head pointer
    if (isFull()) {
        head_ = retreat(head_);
    }

    tail_ = retreat(tail_);
    buffer_[tail_] = std::move(snapshot);

    // Maintain the the retain amount of snapshots behind read_
    // We expect pushTail only to be used after readBackward, which would make this unnecessary,
    // but we keep it here for safety.
    if (distance(tail_, read_) > retain_count_) {
        read_ = retreat(read_);
    }

    cv_.notify_all();
    return true;
}

std::optional<double> SystemRingBuffer::headTime() const {
    std::unique_lock lock(mtx_);
    if (isEmpty()) return std::nullopt;
    return buffer_[retreat(head_)]->time;
}

std::optional<double> SystemRingBuffer::tailTime() const {
    std::unique_lock lock(mtx_);
    if (isEmpty()) return std::nullopt;
    return buffer_[tail_]->time;
}

std::optional<SystemSnapshotPtr> SystemRingBuffer::readForward() {
    std::unique_lock lock(mtx_);
    if (hasReaderCaughtUp()) return std::nullopt;

    auto result = buffer_[read_];
    read_ = advance(read_);

    // Ensure we maintain retain_count_ behind read_
    bool space_freed = false;
    while (distance(tail_, read_) > retain_count_) {
        tail_ = advance(tail_);
        space_freed = true;
    }

    if (space_freed) cv_.notify_all();

    return result;
}

std::optional<SystemSnapshotPtr> SystemRingBuffer::readBackward() {
    std::unique_lock lock(mtx_);
    if (distance(tail_, read_) < 1) return std::nullopt;  // Can't go before tail

    read_ = retreat(read_);
    return buffer_[read_];
}

void SystemRingBuffer::clear() {
    std::unique_lock lock(mtx_);
    tail_ = read_ = head_ = 0;
    buffer_.assign(buffer_.size(), nullptr);
    is_shutting_down_ = false;
    cv_.notify_all();
}

void SystemRingBuffer::shutdown() {
    std::unique_lock lock(mtx_);
    is_shutting_down_ = true;
    cv_.notify_all();
}

bool SystemRingBuffer::isFull() const { return advance(head_) == tail_; }

bool SystemRingBuffer::isEmpty() const { return tail_ == head_; }

bool SystemRingBuffer::hasReaderCaughtUp() const { return read_ == head_; }

size_t SystemRingBuffer::size() const { return distance(tail_, head_); }

size_t SystemRingBuffer::advance(size_t idx) const { return (idx + 1) % buffer_.size(); }

size_t SystemRingBuffer::retreat(size_t idx) const {
    return (idx + buffer_.size() - 1) % buffer_.size();
}

size_t SystemRingBuffer::distance(size_t from, size_t to) const {
    if (to >= from) return to - from;
    return to + buffer_.size() - from;
}
