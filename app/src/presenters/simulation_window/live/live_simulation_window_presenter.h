#pragma once

#include <QObject>
#include <QTimer>
#include <chrono>
#include <memory>

#include "core/dataflow/blocking_queue.h"
#include "core/dataflow/debug_info.h"
#include "core/dataflow/latest_value_slot.h"
#include "core/dataflow/snapshot.h"
#include "presenters/simulation_window/simulation_window_presenter.h"
#include "views/simulation_window/live/i_live_simulation_window_view.h"
#include "workers/queue_storage_worker.h"

class LiveSimulationWindowPresenter : public SimulationWindowPresenter {
    Q_OBJECT
public:
    explicit LiveSimulationWindowPresenter(
        ILiveSimulationWindowView* view,
        std::shared_ptr<LatestValueSlot<SystemSnapshot>> rendering_snapshot,
        std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue,
        std::shared_ptr<LiveDebugInfo> debug_info,
        QObject* parent = nullptr);
    ~LiveSimulationWindowPresenter() override;

private:
    void updateDebugInfo();

    ILiveSimulationWindowView* view_;

    QueueStorageWorkerBase* chart_worker_ = nullptr;
    QThread* chart_thread_ = nullptr;
    std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue_;

    QTimer* debug_info_timer_;
    std::shared_ptr<LiveDebugInfo> debug_info_;
    std::chrono::steady_clock::time_point last_debug_info_update_time_;
    int previous_step_count_ = 0;
};
