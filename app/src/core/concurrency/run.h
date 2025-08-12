#pragma once

#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <type_traits>
#include <utility>

namespace app::concurrency {
/**
 * @brief Runs a function asynchronously and invokes a handler with the result on the context
 * object's thread.
 *
 * This function encapsulates the pattern of using QtConcurrent::run with a QFutureWatcher
 * to get a result back on a specific thread without blocking. The watcher is automatically
 * deleted after the handler is invoked.
 *
 * @tparam Callable The type of the function to run in the background.
 * @tparam ResultHandler The type of the callback function to handle the result.
 * @tparam Args The types of the arguments to pass to the background function.
 * @param context A QObject* that provides the context for the callback. The ResultHandler will be
 * executed in this object's thread.
 * @param func The function to execute in a background thread.
 * @param result_handler The callback to execute with the function's result.
 * @param args The arguments to forward to the background function.
 */
template <typename Callable, typename ResultHandler, typename... Args>
void run(QObject* context, Callable&& func, ResultHandler&& result_handler, Args&&... args) {
    using ResultType = std::invoke_result_t<Callable, Args...>;

    // The watcher's parent is now the provided context object.
    auto* watcher = new QFutureWatcher<ResultType>(context);

    QObject::connect(watcher,
                     &QFutureWatcher<ResultType>::finished,
                     context,
                     [watcher, result_handler = std::forward<ResultHandler>(result_handler)]() {
                         result_handler(watcher->result());
                         watcher->deleteLater();
                     });

    watcher->setFuture(
        QtConcurrent::run(std::forward<Callable>(func), std::forward<Args>(args)...));
}

}  // namespace app::concurrency
