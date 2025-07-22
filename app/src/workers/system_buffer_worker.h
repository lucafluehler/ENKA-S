#pragma once

#include <QObject>
#include <QThread>
#include <atomic>
#include <filesystem>
#include <memory>

#include "core/files/system_snapshot_stream.h"
#include "core/system_ring_buffer.h"

/**
 * @brief Worker for writing to the system ring buffer by parsing files.
 */
class SystemBufferWorker : public QObject {
    Q_OBJECT

public:
    SystemBufferWorker(std::shared_ptr<SystemRingBuffer> buffer,
                       const std::filesystem::path& file_path,
                       QObject* parent = nullptr);

    /**
     * @brief Aborts the worker, stopping any further processing and shutting down the buffer.
     */
    void abort();

    /**
     * @brief Requests parsing a snapshot at the tail of the buffer and pushing it to the tail.
     */
    void requestStepBackward();

public slots:
    /**
     * @brief Runs the worker, processing snapshots from the file and pushing them to the buffer.
     * It will continue until an abort signal is received.
     */
    void run();

private:
    void stepBackward();
    void stepForward();

    std::shared_ptr<SystemRingBuffer> buffer_;
    std::filesystem::path file_path_;

    std::atomic<int> backward_steps_ = 0;
    std::atomic<bool> stop_requested_ = false;

    std::unique_ptr<SystemSnapshotStream> stream_ = nullptr;
    double last_timestamp_ = 0.0;
};