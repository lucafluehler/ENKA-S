#pragma once

#include <memory>
#include <mutex>
#include <utility>

/**
 * @brief A thread-safe, single-element queue with overwrite semantics.
 * @tparam T The type of the object stored in the shared_ptr.
 */
template <typename T>
class LatestValueSlot final {
public:
    using Ptr = std::shared_ptr<T>;

    LatestValueSlot() = default;

    // Prevent copying and moving to ensure safety.
    LatestValueSlot(const LatestValueSlot&) = delete;
    LatestValueSlot& operator=(const LatestValueSlot&) = delete;
    LatestValueSlot(LatestValueSlot&&) = delete;
    LatestValueSlot& operator=(LatestValueSlot&&) = delete;

    /**
     * @brief Writes data to the slot, overwriting any previous data.
     *
     * This method is intended for the producer thread.
     * @param data The shared_ptr to the data to be stored.
     */
    void set(Ptr data) {
        std::lock_guard<std::mutex> lock(mutex_);
        slot_ = std::move(data);
    }

    /**
     * @brief Retrieves the data from the slot, leaving it empty.
     *
     * This method is intended for the consumer thread. It retrieves the
     * currently stored shared_ptr and replaces it with nullptr atomically.
     *
     * @return The shared_ptr that was in the slot. Returns nullptr if the
     *         slot was already empty.
     */
    Ptr take() {
        std::lock_guard<std::mutex> lock(mutex_);
        Ptr data;
        std::swap(data, slot_);
        return data;
    }

private:
    Ptr slot_ = nullptr;
    mutable std::mutex mutex_;
};
