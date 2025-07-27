#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>

template <typename T>
class BlockingQueue {
public:
    explicit BlockingQueue(size_t max_size) : max_size_(max_size), current_size_(0) {}

    /**
     * @brief Pushes an item into the queue, blocking until space is available.
     * @param item The item to be pushed into the queue.
     */
    void pushBlocking(T item) {
        std::unique_lock lock(mutex_);
        cond_full_.wait(
            lock, [&]() { return current_size_.load(std::memory_order_relaxed) < max_size_; });
        buffer_.push_back(std::move(item));
        current_size_.fetch_add(1, std::memory_order_relaxed);
        lock.unlock();
        cond_empty_.notify_one();
    }

    /**
     * @brief Pops an item from the queue, blocking until an item is available.
     * @return The item popped from the queue.
     */
    T popBlocking() {
        std::unique_lock lock(mutex_);
        cond_empty_.wait(lock, [&]() { return !buffer_.empty(); });
        T item = std::move(buffer_.front());
        buffer_.pop_front();
        current_size_.fetch_sub(1, std::memory_order_relaxed);
        lock.unlock();
        cond_full_.notify_one();
        return item;
    }

    /**
     * @brief Returns the number of items currently in the queue.
     * @return The current size of the queue.
     */
    [[nodiscard]] size_t size() const { return current_size_.load(std::memory_order_relaxed); }

private:
    mutable std::mutex mutex_;
    std::condition_variable cond_full_;
    std::condition_variable cond_empty_;
    std::deque<T> buffer_;
    size_t max_size_;
    std::atomic<size_t> current_size_;
};
