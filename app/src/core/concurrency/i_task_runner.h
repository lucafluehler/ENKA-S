#pragma once

#include <QObject>
#include <functional>

class ITaskRunner {
public:
    virtual ~ITaskRunner() = default;

    template <typename Func, typename Callback, typename... Args>
    void run(QObject* context, Func task, Callback on_done, Args&&... args) {
        run_impl(context, [=]() {
            using ResultType = decltype(std::invoke(task, args...));

            ResultType result = std::invoke(task, args...);

            QMetaObject::invokeMethod(
                context, [on_done, result]() { on_done(result); }, Qt::QueuedConnection);
        });
    }

private:
    virtual void run_impl(QObject* context, std::function<void()>&& work) = 0;
};
