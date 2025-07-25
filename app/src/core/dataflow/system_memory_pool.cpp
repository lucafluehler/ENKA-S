#include "core/dataflow/system_memory_pool.h"

#include <stdexcept>

SystemMemoryPool::SystemMemoryPool(size_t pool_size, size_t particle_count) {
    if (pool_size == 0) {
        throw std::invalid_argument("Pool size must be greater than zero.");
    }

    // Pre-allocate all the buffers and store their raw pointers in the queue.
    for (size_t i = 0; i < pool_size; ++i) {
        auto* buffer = new enkas::data::System();
        try {
            buffer->resize(particle_count);
        } catch (const std::bad_alloc& e) {
            delete buffer;
            throw std::runtime_error("Failed to allocate system memory pool: not enough memory.");
        }
        available_buffers_.push(buffer);
    }
}

SystemMemoryPool::~SystemMemoryPool() {
    while (!available_buffers_.empty()) {
        delete available_buffers_.front();
        available_buffers_.pop();
    }
}

std::shared_ptr<enkas::data::System> SystemMemoryPool::acquireBuffer() {
    // Lock the mutex to safely access the queue.
    std::unique_lock<std::mutex> lock(mtx_);

    // This will only proceed when the queue is NOT empty.
    cv_.wait(lock, [this] { return !available_buffers_.empty(); });

    enkas::data::System* buffer_ptr = available_buffers_.front();
    available_buffers_.pop();

    auto deleter = [this](enkas::data::System* ptr_to_return) {
        this->returnBuffer(ptr_to_return);
    };

    return {buffer_ptr, deleter};
}

void SystemMemoryPool::returnBuffer(enkas::data::System* buffer) {
    {
        std::scoped_lock<std::mutex> lock(mtx_);
        available_buffers_.push(buffer);
    }  // Lock is released here.

    // For waking up a blocked acquireBuffer() call.
    cv_.notify_one();
}
