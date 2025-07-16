#pragma once

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>

#include "core/data_ptr.h"

class BlockingQueue {
public:
    void pushBlocking(std::shared_ptr<const DataPtr> snapshot) {
        std::unique_lock lock(mutex_);
        cond_full_.wait(lock, [&]() { return buffer_.size() < max_size_; });
        buffer_.push_back(std::move(snapshot));
        cond_empty_.notify_one();
    }

    std::shared_ptr<const DataPtr> popBlocking() {
        std::unique_lock lock(mutex_);
        cond_empty_.wait(lock, [&]() { return !buffer_.empty(); });
        auto item = buffer_.front();
        buffer_.pop_front();
        cond_full_.notify_one();
        return item;
    }

private:
    std::mutex mutex_;
    std::condition_variable cond_full_;
    std::condition_variable cond_empty_;
    std::deque<std::shared_ptr<const DataPtr>> buffer_;
    size_t max_size_ = 256;
};
