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
        cond_full_.wait(lock, [&]() { return buffer_.size() < max_size_; });
        buffer_.push_back(std::move(item));
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
        cond_full_.notify_one();
        return item;
    }

private:
    std::mutex mutex_;
    std::condition_variable cond_full_;
    std::condition_variable cond_empty_;
    std::deque<T> buffer_;
    size_t max_size_;
};
