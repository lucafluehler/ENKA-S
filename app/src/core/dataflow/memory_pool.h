#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>

/**
 * @brief A concept to check if a type T has a reset() method.
 *
 * This is used to ensure that the type T can be reset before returning it to the pool.
 */
template <typename T>
concept Resettable = requires(T t) {
    { t.reset() } -> std::same_as<void>;
};

/**
 * @brief A thread-safe memory pool for managing buffers of type T.
 *
 * This class allows preallocation of a specified number of buffers and provides a method to
 * acquire a buffer. When the buffer is no longer needed, it can be returned to the pool.
 *
 * @tparam T The type of objects stored in the pool.
 * @tparam InitArgs The types of arguments used to initialize the T objects.
 */
template <typename T, typename... InitArgs>
class MemoryPool {
public:
    /**
     * @brief Constructs a MemoryPool with a specified size.
     * @param pool_size The number of buffers to preallocate in the pool.
     * @param args Arguments to initialize the T objects.
     */
    explicit MemoryPool(size_t pool_size, InitArgs... args) {
        if (pool_size == 0) throw std::invalid_argument("Pool size must be greater than zero.");

        for (size_t i = 0; i < pool_size; ++i) buffers_.push(new T(args...));
    }

    ~MemoryPool() {
        while (!buffers_.empty()) {
            delete buffers_.front();
            buffers_.pop();
        }
    }

    /**
     * @brief Acquires a buffer from the pool.
     *
     * If the pool is empty, this call will block until a buffer is returned
     * by another thread.
     *
     * @return A std::shared_ptr to a T object that will automatically return the buffer to the pool
     *         when it goes out of scope. If T is Resettable, it will call reset() on the object
     *         before returning it to the pool.
     */
    [[nodiscard]] std::shared_ptr<T> acquire() {
        std::unique_lock lock(mtx_);
        cv_.wait(lock, [&] { return !buffers_.empty(); });

        T* ptr = buffers_.front();
        buffers_.pop();

        return {ptr, [this](T* p) {
                    if constexpr (Resettable<T>) p->reset();  // Only if it has reset()
                    returnBuffer(p);
                }};
    }

    /**
     * @brief Returns the number of currently available buffers in the pool.
     * @return The number of available (not in-use) buffers.
     */
    [[nodiscard]] size_t size() const {
        std::scoped_lock lock(mtx_);
        return buffers_.size();
    }

private:
    void returnBuffer(T* ptr) {
        {
            std::scoped_lock lock(mtx_);
            buffers_.push(ptr);
        }
        cv_.notify_one();
    }

    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<T*> buffers_;
};
