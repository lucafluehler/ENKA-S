#pragma once
#include <QThreadPool>

#include "services/task_runner/i_task_runner.h"

class ConcurrentRunner : public ITaskRunner {
private:
    void run_impl(QObject* context, std::function<void()>&& work) override {
        QThreadPool::globalInstance()->start(std::move(work));
    }
};
