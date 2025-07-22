#pragma once

#include <QObject>
#include <QTimer>
#include <atomic>
#include <chrono>
#include <memory>

#include "core/blocking_queue.h"
#include "core/snapshot.h"

class ISimulationWindowView;

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
    ~SimulationWindowPresenter() override = default;

    /**
     * @brief Enumeration for the different modes of the simulation window.
     * Uninitialized: The presenter has not been initialized.
     * Live: The presenter is in live mode, receiving real-time data.
     * Replay: The presenter is in replay mode, reading from recorded data.
     */
    enum class Mode { Uninitialized, Live, Replay };

    /**
     * @brief Initializes the presenter for live mode.
     * @param render_queue_slot Pointer to the atomic slot for the render queue.
     * @param chart_queue Shared pointer to the diagnostics chart queue.
     * @param simulation_duration Duration of the simulation in seconds.
     */
    void initLiveMode(std::atomic<SystemSnapshotPtr>* render_queue_slot,
                      std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue,
                      double simulation_duration);

    /**
     * @brief Initializes the presenter for replay mode.
     * @param system_ring_buffer Shared pointer to the system ring buffer for replaying system data.
     * @param timestamps Shared pointer to a vector of timestamps for the replay.
     * @param diagnostics_series Shared pointer to the diagnostics data for the charts.
     */
    void initReplayMode(std::atomic<SystemSnapshotPtr>* render_queue_slot,
                        std::shared_ptr<std::vector<double>> timestamps,
                        std::shared_ptr<DiagnosticsSeries> diagnostics_series);

    /**
     * @brief Returns the current mode of the presenter.
     * @return The current mode.
     */
    Mode getMode() const { return mode_; };

public slots:
    /**
     * @brief Updates the rendering of the simulation window.
     * This method is called periodically to update the view with the latest system snapshot.
     */
    void updateRendering();

    /**
     * @brief Updates the charts with the latest diagnostics data.
     */
    void updateCharts();

private:
    ISimulationWindowView* view_;

    Mode mode_;
    QTimer* render_timer_;

    const int target_fps_ = 120;
    std::chrono::steady_clock::time_point last_debug_info_update_time_;
    int frame_count_ = 0;
    int snapshot_count_ = 0;

    std::atomic<SystemSnapshotPtr>* render_queue_slot_;

    // For live mode
    std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue_;
};
