#pragma once

#include <QObject>
#include <functional>

#include "core/blocking_queue.h"

template <typename SnapshotPtr>
class QueueStorageWorker : public QObject {
    Q_OBJECT
public:
    using SaveFn = std::function<void(const SnapshotPtr&)>;

    QueueStorageWorker(std::shared_ptr<BlockingQueue<SnapshotPtr>> queue,
                       SaveFn save_function,
                       QObject* parent = nullptr)
        : QObject(parent), queue_(std::move(queue)), save_function_(std::move(save_function)) {}

public slots:
    void run() {
        while (true) {
            auto snapshot = queue_->popBlocking();
            if (!snapshot) break;  // sentinel on abort()
            save_function_(snapshot);
        }
        emit workFinished();
    }

    void abort() {
        queue_->pushBlocking(nullptr);  // wake up run() by placing a nullptr sentinel
    }

signals:
    void workFinished();

private:
    std::shared_ptr<BlockingQueue<SnapshotPtr>> queue_;
    SaveFn save_function_;  // Function which will be called to save the snapshot
};