#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <vector>

#include "core/snapshot.h"

/**
 * @brief A thread-safe ring buffer for storing and managing system snapshots.
 */
class SystemRingBuffer {
public:
    explicit SystemRingBuffer(size_t capacity, size_t retain_count);

    /**
     * @brief Pushes a snapshot to the head of the buffer.
     * If the buffer is full, it will wait until space is available.
     * @param snapshot The snapshot to push.
     * @return true if the snapshot was successfully pushed, false if shutting down.
     */
    bool pushHead(SystemSnapshotPtr snapshot);

    /**
     * @brief Pushes a snapshot to the tail of the buffer.
     * If the buffer is full, it will retreat the head pointer.
     * @param snapshot The snapshot to push.
     * @return true if the snapshot was successfully pushed.
     */
    bool pushTail(SystemSnapshotPtr snapshot);

    /**
     * @brief Reads the time of the snapshot right before the head of the buffer.
     * @return The time of the head snapshot, or std::nullopt if the buffer is empty.
     */
    std::optional<double> headTime() const;

    /**
     * @brief Reads the time of the snapshot at the tail of the buffer.
     * @return The time of the tail snapshot, or std::nullopt if the buffer is empty.
     */
    std::optional<double> tailTime() const;

    /**
     * @brief Reads the next snapshot in the buffer, moving the read pointer forward.
     * @return The next snapshot, or std::nullopt if there are no more snapshots.
     */
    std::optional<SystemSnapshotPtr> readForward();

    /**
     * @brief Reads the previous snapshot in the buffer, moving the read pointer backward.
     * @return The previous snapshot, or std::nullopt if there are no more snapshots.
     */
    std::optional<SystemSnapshotPtr> readBackward();

    /**
     * @brief Clears the buffer, removing all snapshots.
     */
    void clear();

    /**
     * @brief Shuts down the buffer, preventing further writes and notifying waiting threads.
     */
    void shutdown();

    /**
     * @brief Checks if the buffer is full.
     * @return true if the buffer is full, false otherwise.
     */
    bool isFull() const;

    /**
     * @brief Checks if the buffer is empty.
     * @return true if the buffer is empty, false otherwise.
     */
    bool isEmpty() const;

    /**
     * @brief Checks if the reader has caught up with the head of the buffer.
     * @return true if the reader is caught up, false otherwise.
     */
    bool hasReaderCaughtUp() const;

    /**
     * @brief Gets the current size of the buffer.
     * @return The number of snapshots currently in the buffer.
     */
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
