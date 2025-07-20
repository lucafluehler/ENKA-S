#pragma once

#include <QObject>
#include <functional>
#include <memory>

#include "core/blocking_queue.h"

class QueueStorageWorkerBase : public QObject {
    Q_OBJECT

public:
    explicit QueueStorageWorkerBase(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~QueueStorageWorkerBase() = default;

public slots:
    virtual void run() = 0;
    virtual void abort() = 0;

signals:
    void workFinished();
};

template <typename SnapshotPtr>
class QueueStorageWorker : public QueueStorageWorkerBase {
public:
    using SaveFn = std::function<void(const SnapshotPtr&)>;

    QueueStorageWorker(std::shared_ptr<BlockingQueue<SnapshotPtr>> queue,
                       SaveFn save_function,
                       QObject* parent = nullptr)
        : QueueStorageWorkerBase(parent),
          queue_(std::move(queue)),
          save_function_(std::move(save_function)) {}

public:
    /**
     * @brief Runs the worker, processing snapshots from the queue and saving them using the
     * provided function. Continues until an abort signal is received in the form of a nullptr
     * snapshot.
     */
    void run() override {
        while (true) {
            auto snapshot = queue_->popBlocking();
            if (!snapshot) break;  // sentinel on abort()
            save_function_(snapshot);
        }
        emit workFinished();
    }

    /**
     * @brief Aborts the worker by pushing a nullptr to the queue, which will break the processing
     * loop in run().
     */
    void abort() override {
        queue_->pushBlocking(nullptr);  // sentinel to stop processing
    }

private:
    std::shared_ptr<BlockingQueue<SnapshotPtr>> queue_;
    SaveFn save_function_;
};
