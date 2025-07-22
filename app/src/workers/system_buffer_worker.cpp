#include "system_buffer_worker.h"

#include "core/files/file_parse_logic.h"
#include "core/snapshot.h"

SystemBufferWorker::SystemBufferWorker(std::shared_ptr<SystemRingBuffer> buffer,
                                       const std::filesystem::path& file_path,
                                       QObject* parent)
    : QObject(parent), buffer_(std::move(buffer)), file_path_(file_path) {}

void SystemBufferWorker::abort() {
    stop_requested_.store(true, std::memory_order_release);
    buffer_->shutdown();
}

void SystemBufferWorker::requestStepBackward() {
    backward_steps_.fetch_add(1, std::memory_order_release);
}

void SystemBufferWorker::run() {
    while (!stop_requested_.load(std::memory_order_acquire)) {
        while (backward_steps_.fetch_sub(1, std::memory_order_acquire) > 0) {
            if (stop_requested_.load(std::memory_order_acquire)) break;
            stepBackward();
        }

        if (!stop_requested_.load(std::memory_order_acquire)) {
            stepForward();
        }
    }
}

void SystemBufferWorker::stepBackward() {
    if (auto tail_time = buffer_->tailTime()) {
        if (auto snapshot = FileParseLogic::parsePreviousSystemSnapshot(file_path_, *tail_time)) {
            buffer_->pushTail(std::make_shared<SystemSnapshot>(*snapshot));
        }
    }
}

void SystemBufferWorker::stepForward() {
    if (auto snapshot = FileParseLogic::parseNextSystemSnapshot(file_path_, last_timestamp_)) {
        last_timestamp_ = snapshot->time;
        buffer_->pushHead(std::make_shared<SystemSnapshot>(*snapshot));
    }
}
