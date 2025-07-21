#pragma once

#include <QObject>
#include <QThread>
#include <atomic>
#include <filesystem>
#include <memory>

#include "core/system_ring_buffer.h"

class SystemBufferWorker : public QObject {
    Q_OBJECT

public:
    SystemBufferWorker(std::shared_ptr<SystemRingBuffer> buffer,
                       const std::filesystem::path& file_path,
                       QObject* parent = nullptr);

    void abort();
    void requestStepBackward();

public slots:
    void run();

private:
    void stepBackward();
    void stepForward();

    std::shared_ptr<SystemRingBuffer> buffer_;
    std::filesystem::path file_path_;

    std::atomic<int> backward_steps_{0};
    std::atomic<bool> stop_requested_{false};

    double last_timestamp_{0.0};
};