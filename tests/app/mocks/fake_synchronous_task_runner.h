#pragma once

#include <gmock/gmock.h>

#include "services/task_runner/i_task_runner.h"

class FakeSynchronousTaskRunner : public ITaskRunner {
private:
    void run_impl(QObject* context, std::function<void()>&& work) override { work(); }
};
