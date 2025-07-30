#pragma once

#include <QObject>
#include <QTimer>
#include <atomic>
#include <chrono>
#include <memory>

#include "core/dataflow/blocking_queue.h"
#include "core/dataflow/debug_info.h"
#include "core/dataflow/snapshot.h"
#include "views/simulation_window/i_simulation_window_view.h"
#include "workers/queue_storage_worker.h"

/**
 * @brief Handles the logic for the simulation window, including rendering updates and managing
 * different modes (live or file).
 */
class SimulationWindowPresenter : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Initializes the presenter with a view.
     * @param view Pointer to the view interface for the simulation window.
     * @param parent Optional parent QObject.
     */
    explicit SimulationWindowPresenter(ISimulationWindowView* view, QObject* parent = nullptr);
    ~SimulationWindowPresenter() override;

    /**
     * @brief Enumeration for the different modes of the simulation window.
     * Uninitialized: The presenter has not been initialized.
     * Live: The presenter is in live mode, receiving real-time data.
     * Replay: The presenter is in replay mode, reading from recorded data.
     */
    enum class Mode { Uninitialized, Live, Replay };

    /**
     * @brief Initializes the presenter for live mode.
     * @param rendering_snapshot Shared pointer to the snapshot to be rendered.
     * @param chart_queue Shared pointer to the diagnostics chart queue.
     * @param debug_info Shared pointer to the debug information for live mode.
     */
    void initLiveMode(std::shared_ptr<std::atomic<SystemSnapshotPtr>> rendering_snapshot,
                      std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue,
                      std::shared_ptr<LiveDebugInfo> debug_info);

    /**
     * @brief Initializes the presenter for replay mode.
     * @param rendering_snapshot Shared pointer to the snapshot to be rendered.
     * @param timestamps Shared pointer to a vector of timestamps for the replay.
     * @param diagnostics_series Shared pointer to the diagnostics data for the charts.
     */
    void initReplayMode(std::shared_ptr<std::atomic<SystemSnapshotPtr>> rendering_snapshot,
                        std::shared_ptr<std::vector<double>> timestamps,
                        std::shared_ptr<DiagnosticsSeries> diagnostics_series);

    /**
     * @brief Returns the current mode of the presenter.
     * @return The current mode.
     */
    Mode getMode() const { return mode_; };

signals:
    /** @signal
     * @brief Emitted when new diagnostics data is ready to be displayed.
     */
    void diagnosticsReady(DiagnosticsSnapshotPtr snapshot);

public slots:
    /**
     * @brief Updates the rendering of the simulation window.
     * This method is called periodically to update the view with the latest system snapshot.
     */
    void updateRendering();

    /**
     * @brief Updates the charts with the latest diagnostics data.
     */
    void updateDiagnostics(DiagnosticsSnapshotPtr diagnostics_snapshot);

    /**
     * @brief Handles changes in the target frames per second (FPS).
     */
    void onFpsChanged() { render_timer_->setInterval(1000 / view_->getTargetFPS()); }

private:
    void setupChartWorker();

    ISimulationWindowView* view_;

    Mode mode_;
    QTimer* render_timer_;

    std::chrono::steady_clock::time_point last_debug_info_update_time_;
    int frame_count_ = 0;
    int previous_step_count_ = 0;

    std::shared_ptr<std::atomic<SystemSnapshotPtr>> rendering_snapshot_;

    // For live mode
    QueueStorageWorkerBase* chart_worker_ = nullptr;
    QThread* chart_thread_ = nullptr;

    std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue_;
    std::shared_ptr<LiveDebugInfo> live_debug_info_;
};
