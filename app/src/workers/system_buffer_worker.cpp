#include "system_buffer_worker.h"

#include <enkas/logging/logger.h>

#include "core/dataflow/snapshot.h"

SystemBufferWorker::SystemBufferWorker(std::shared_ptr<SystemRingBuffer> buffer,
                                       const std::filesystem::path& file_path,
                                       QObject* parent)
    : QObject(parent), buffer_(std::move(buffer)), file_path_(file_path) {
    stream_ = std::make_unique<SystemSnapshotStream>(file_path_);
    if (!stream_->initialize()) {
        ENKAS_LOG_ERROR("Failed to initialize SystemSnapshotStream for file: {}",
                        file_path_.string());
    }
}

void SystemBufferWorker::abort() {
    stop_requested_.store(true, std::memory_order_release);
    buffer_->shutdown();
}

void SystemBufferWorker::requestStepBackward() {
    backward_steps_.fetch_add(1, std::memory_order_release);
}

void SystemBufferWorker::run() {
    if (!stream_ || !stream_->isInitialized()) {
        ENKAS_LOG_ERROR("SystemSnapshotStream is not initialized. Cannot run worker.");
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

        if (!stop_requested_.load(std::memory_order_acquire)) {
            stepForward();
        }
    }
}

void SystemBufferWorker::stepBackward() {
    if (auto tail_time = buffer_->tailTime()) {
        if (auto snapshot = stream_->getPrecedingSnapshot(*tail_time)) {
            buffer_->pushTail(std::make_shared<SystemSnapshot>(*snapshot));
        }
    }
}

void SystemBufferWorker::stepForward() {
    if (auto snapshot = stream_->getNextSnapshot()) {
        last_timestamp_ = snapshot->time;
        buffer_->pushHead(std::make_shared<SystemSnapshot>(*snapshot));
    }
}
