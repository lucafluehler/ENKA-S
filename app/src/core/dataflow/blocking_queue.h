#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>

template <typename T>
class BlockingQueue {
public:
    explicit BlockingQueue(size_t max_size) : max_size_(max_size) {}

    /**
     * @brief Pushes an item into the queue, blocking until space is available.
     * @param item The item to be pushed into the queue.
     */
    void pushBlocking(T item) {
        std::unique_lock lock(mutex_);
        // Wait until the buffer is not full
        cond_full_.wait(lock, [&]() { return buffer_.size() < max_size_; });

        buffer_.push_back(std::move(item));

        lock.unlock();
        cond_empty_.notify_one();
    }

    /**
     * @brief Pops an item from the queue, blocking until an item is available.
     * @return The item popped from the queue.
     */
    [[nodiscard]] T popBlocking() {
        std::unique_lock lock(mutex_);
        // Wait until the buffer is not empty
        cond_empty_.wait(lock, [&]() { return !buffer_.empty(); });

        T item = std::move(buffer_.front());
        buffer_.pop_front();

        // Manual unlock before notify
        lock.unlock();
        cond_full_.notify_one();

        return item;
    }

    /**
     * @brief Returns the number of items currently in the queue.
     * @return The current size of the queue.
     * @note This locks the mutex, so it is a thread-safe but potentially blocking call.
     */
    [[nodiscard]] size_t size() const {
        std::scoped_lock lock(mutex_);  // scoped_lock is great for simple acquisitions
        return buffer_.size();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable cond_full_;
    std::condition_variable cond_empty_;
    std::deque<T> buffer_;
    const size_t max_size_;
};
