#pragma once

#include <enkas/data/system.h>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

/**
 * @class SystemMemoryPool
 * @brief Manages a fixed-size pool of reusable enkas::data::System objects.
 *
 * This class allocates a number of buffers up-front and then "lends" them out as std::shared_ptrs.
 *
 * The shared_ptrs are equipped with a custom deleter that, instead of freeing
 * the memory, returns the buffer to the pool, making it available for reuse.
 *
 * This class is thread-safe.
 */
class SystemMemoryPool {
public:
    /**
     * @brief Constructs the pool and pre-allocates all necessary buffers.
     * @param pool_size The number of enkas::data::System buffers to create in the pool.
     * @param particle_count The number of particles each buffer should be sized for.
     */
    SystemMemoryPool(size_t pool_size, size_t particle_count);

    /**
     * @brief Destructor that safely cleans up all allocated buffers.
     */
    ~SystemMemoryPool();

    // Disable copy and move semantics to ensure single ownership of the pool.
    SystemMemoryPool(const SystemMemoryPool&) = delete;
    SystemMemoryPool& operator=(const SystemMemoryPool&) = delete;
    SystemMemoryPool(SystemMemoryPool&&) = delete;
    SystemMemoryPool& operator=(SystemMemoryPool&&) = delete;

    /**
     * @brief Acquires a buffer from the pool.
     *
     * If the pool is empty, this call will block until a buffer is returned
     * by another thread.
     *
     * @return A std::shared_ptr to a enkas::data::System buffer. When this shared_ptr's
     *         reference count drops to zero, the buffer is automatically returned
     *         to the pool.
     */
    [[nodiscard]] std::shared_ptr<enkas::data::System> acquireBuffer();

private:
    /**
     * @brief The custom deleter function. Returns a raw pointer to the pool.
     * @param buffer The raw pointer to the enkas::data::System object to return.
     */
    void returnBuffer(enkas::data::System* buffer);

    std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<enkas::data::System*> available_buffers_;
};
