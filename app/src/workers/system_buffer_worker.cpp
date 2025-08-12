#include "system_buffer_worker.h"

#include <enkas/logging/logger.h>

#include "core/dataflow/snapshot.h"

void SystemBufferWorker::run() {
    if (!stream_ || !stream_->initialize()) {
        ENKAS_LOG_ERROR("Failed to initialize SystemSnapshotStream for file: {}",
                        file_path_.string());
        return;
    }

    while (!stop_requested_.load(std::memory_order_acquire)) {
        int steps;
        while ((steps = backward_steps_.load(std::memory_order_acquire)) > 0) {
            if (backward_steps_.compare_exchange_weak(
                    steps, steps - 1, std::memory_order_acquire, std::memory_order_relaxed)) {
                if (stop_requested_.load(std::memory_order_acquire)) break;
                stepBackward();
            }
        }

        const float jump_fraction = jump_fraction_.load(std::memory_order_acquire);
        if (!std::isnan(jump_fraction)) {
            jump();
            jump_fraction_.store(jump_unset_, std::memory_order_release);
        }

        if (!stop_requested_.load(std::memory_order_acquire)) {
            stepForward();
        }
    }
}

void SystemBufferWorker::stepBackward() {
    if (auto tail_time = buffer_->tailTime()) {
        if (auto snapshot = stream_->getPrecedingSnapshot(*tail_time)) {
            bool success = buffer_->pushTail(std::make_shared<SystemSnapshot>(*snapshot));
            if (buffer_->isFull() && success) stream_->retreatIndexIterator();
        }
    }
}

void SystemBufferWorker::stepForward() {
    // If the buffer is full, we cannot push to the head.
    if (buffer_->isFull()) return;

    if (auto snapshot = stream_->getNextSnapshot()) {
        last_timestamp_ = snapshot->time;
        bool success = buffer_->pushHead(std::make_shared<SystemSnapshot>(*snapshot));

        // If pushing failed for any reason, retreat the index iterator to ensure no snapshot is
        // dropped.
        if (!success) stream_->retreatIndexIterator();
    }
}

void SystemBufferWorker::jump() {
    float jump_fraction = jump_fraction_.load(std::memory_order_acquire);
    if (std::isnan(jump_fraction)) return;  // No valid jump requested

    if (auto snapshot = stream_->getSnapshotAtFraction(jump_fraction)) {
        last_timestamp_ = snapshot->time;
        buffer_->clear();  // Reset the buffer before jumping
        buffer_->pushHead(std::make_shared<SystemSnapshot>(*snapshot));
    }
}
