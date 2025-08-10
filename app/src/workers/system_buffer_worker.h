#pragma once

#include <QObject>
#include <QThread>
#include <atomic>
#include <filesystem>
#include <limits>
#include <memory>

#include "core/dataflow/system_ring_buffer.h"
#include "core/files/system_snapshot_stream.h"

/**
 * @brief Worker for writing to the system ring buffer by parsing files.
 */
class SystemBufferWorker : public QObject {
    Q_OBJECT

public:
    SystemBufferWorker(std::shared_ptr<SystemRingBuffer> buffer,
                       const std::filesystem::path& file_path,
                       QObject* parent = nullptr)
        : QObject(parent),
          buffer_(std::move(buffer)),
          file_path_(file_path),
          stream_(std::make_unique<SystemSnapshotStream>(file_path)) {}

    /**
     * @brief Aborts the worker, stopping any further processing and shutting down the buffer.
     */
    void abort() {
        stop_requested_.store(true, std::memory_order_release);
        buffer_->shutdown();
    }

    /**
     * @brief Requests parsing a snapshot at the tail of the buffer and pushing it to the tail.
     */

    void requestStepBackward() { backward_steps_.fetch_add(1, std::memory_order_release); }

    /**
     * @brief Requests jumping to a specific timestamp in the playback bar. This will reset the
     * buffer.
     */
    void requestJump(double timestamp) { jump_timestamp_.store(timestamp); }

public slots:
    /**
     * @brief Runs the worker, processing snapshots from the file and pushing them to the buffer.
     * It will continue until an abort signal is received.
     */
    void run();

private:
    void stepBackward();
    void stepForward();
    void jump();

    std::shared_ptr<SystemRingBuffer> buffer_;
    std::filesystem::path file_path_;

    std::atomic<int> backward_steps_ = 0;
    std::atomic<bool> stop_requested_ = false;

    static constexpr double jump_unset_ = std::numeric_limits<double>::quiet_NaN();
    std::atomic<double> jump_timestamp_ = jump_unset_;

    std::unique_ptr<SystemSnapshotStream> stream_ = nullptr;
    double last_timestamp_ = 0.0;
};